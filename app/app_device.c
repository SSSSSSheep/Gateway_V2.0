#define _GNU_SOURCE
#include "app_device.h"
#include "app_message.h"
#include "app_common.h"
#include "app_mqtt.h"
#include "app_pool.h"
#include "log/log.h"
#include "cJSON/cJSON.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

#define BUFFER_SIZE 1024

// 整个应用中只有一个Device对象
static Device *device = NULL;

// 发送消息的任务函数（线程池中某个线程函数调用）
static int send_message_task(void *arg)
{
    log_debug("6. send_message_task开始执行");

    // 从上行缓冲区中读取一个字符数组message数据
    char data_buf[128];
    int data_len = app_buffer_read(device->up_buffer, data_buf, sizeof(data_buf));
    log_debug("7. app_buffer_read返回: %d", data_len);
    // 将字符数组message转换为json
    char *json = app_message_chars2json(data_buf, data_len);
    // 将json数据发送给远程
    int ret = app_mqtt_send(json);
    if (ret == -1)
    {
        log_error("发送消息失败");
        return -1;
    }

    log_debug("发送消息成功：%s", json);
    return 0;
}

// 不断从串口文件中读取数据的线程函数
static void *read_thread_func(void *arg)
{
    while (device->is_running)
    {
        // 从串口文件中读取数据
        char data_buf[128];
        ssize_t data_len = read(device->fd, data_buf, sizeof(data_buf));
        // log_debug("------data_len = %d", data_len);
        // 将蓝牙数据转换为字符数组Message
        if (data_len > 0 && device->post_read)
        {
            data_len = device->post_read(data_buf, data_len);
            log_debug("2. post_read 返回: %zd", data_len);
        }
        if (data_len > 0)
        {
            log_debug("3. 准备写入up_buffer: %zd", data_len);

            // 将Message写入上行缓冲区
            int ret = app_buffer_write(device->up_buffer, data_buf, data_len);
            log_debug("4. app_buffer_write返回: %d", ret);

            // 将发送消息的任务交给线程池模块完成 (添加一个发送消息到远程的任务: send_message_task)
            ret = app_pool_addTask(send_message_task, NULL);
            log_debug("5. app_pool_addTask返回: %d", ret);
        }
    }
    return NULL;
}

// 读取下行缓冲区数据处理后写入蓝牙串口文件传递给下游设备的任务函数（线程池中某个线程函数调用）
// 两次写入的时间差 >= 200ms
static int write_thread_func(void *arg)
{
    Device *device = (Device *)arg;
    // 从下行缓冲区读取一个字符数组message数据
    char data_buf[128];
    int data_len = app_buffer_read(device->down_buffer, data_buf, sizeof(data_buf));
    // 将字符数组message转换为蓝牙数据
    if (device->pre_write)
    {
        data_len = device->pre_write(data_buf, data_len);
    }

    // 在准备写入文件前，限制两次写入的时间差>=200ms
    long distance = app_common_getCurrentTime() - device->last_write_time;
    if (distance < 200)
    {
        usleep((200 - distance) * 1000);
    }

    // 将蓝牙数据写入串口文件
    ssize_t len = write(device->fd, data_buf, data_len);
    log_info("write result=%zd, errno=%d", len, errno);
    if (len != data_len)
    {
        log_error("写入蓝牙串口文件失败");
        return -1;
    }

    for (int i = 0; i < data_len; i++)
    {
        printf(" %02X", (unsigned char)data_buf[i]);
    }

    log_debug("写入蓝牙串口文件成功: %.*s", data_len, data_buf);
    // 保存时间
    device->last_write_time = app_common_getCurrentTime();
    return 0;
}

// 处理接收远程消息的回调函数
static int recv_msg_callback(char *json)
{

    log_info("MQTT recv callback: %s", json);

    // json消息转换为字符数组Message
    char data_buf[128];
    int data_len = app_message_json2chars(json, data_buf, sizeof(data_buf));
    log_info("json2chars ret=%d", data_len);

    // 将Message写入下行缓冲区
    int buffer_ret = app_buffer_write(device->down_buffer, data_buf, data_len);
    log_info("down_buffer write ret=%d", buffer_ret);

    // 将后面写入蓝牙串口文件的任务交给线程池模块完成（添加一个将下行缓冲区数据写入蓝牙串口文件的任务）
    int task_ret = app_pool_addTask(write_thread_func, device);
    log_info("add write task ret=%d", task_ret);
    return 0;
}

Device *app_device_init(char *filename)
{
    if (device != NULL)
    {
        return device;
    }
    // 申请Device空间
    device = (Device *)malloc(sizeof(Device));
    if (device == NULL)
    {
        log_error("申请Device空间失败");
        return NULL;
    }

    // 初始化Device中的各个成员
    device->filename = filename;
    device->fd = open(filename, O_RDWR);
    if (device->fd == -1)
    {
        log_error("打开串口文件失败");
        free(device);
        device = NULL;
        return NULL;
    }
    device->up_buffer = app_buffer_init(BUFFER_SIZE);
    device->down_buffer = app_buffer_init(BUFFER_SIZE);
    device->is_running = 0;
    device->last_write_time = 0;
    device->post_read = NULL;
    device->pre_write = NULL;

    // 初始化线程池
    app_pool_init(5);
    // 初始化MQTT模块
    app_mqtt_init();

    // 返回Device指针
    return device;
}

int app_device_start()
{
    if (device->is_running)
    {
        log_debug("读线程已经启动运行……");
        return 0;
    }
    device->is_running = 1;
    // 启动上行流程：启动读线程
    pthread_create(&device->read_thread, NULL, read_thread_func, NULL);
    // 启动下行流程：给MQTT模块注册一个接收远程消息的回调函数
    app_mqtt_registerRecvCallback(recv_msg_callback);
    return 0;
}

void app_device_close()
{

    if (device == NULL)
    {
        return;
    }
    // 取消读线程
    pthread_cancel(device->read_thread);
    pthread_join(device->read_thread, NULL);

    // 释放线程池
    app_pool_destroy();

    // 关闭MQTT
    app_mqtt_close();

    // 关闭文件
    close(device->fd);

    // 释放buffer
    app_buffer_free(device->up_buffer);
    app_buffer_free(device->down_buffer);

    // 释放device
    free(device);
    device = NULL;
}

int app_serial_flush(Device *device)
{
    return tcflush(device->fd, TCIOFLUSH);
}
