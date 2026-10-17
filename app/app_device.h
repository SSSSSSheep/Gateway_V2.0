#if !defined(__APP_DEVICE_H__)
#define __APP_DEVICE_H__

#include "app_buffer.h"
#include <pthread.h>
// 设备的结构体
typedef struct
{
    char *filename;                        // 串口文件名
    int fd;                                // 文件描述符
    Buffer *up_buffer;                     // 上行缓冲区
    Buffer *down_buffer;                   // 下行缓冲区
    pthread_t read_thread;                 // 读取线程
    int is_running;                        // 读线程是否正在运行
    long last_write_time;                  // 上次写入时间
    int (*post_read)(char *data, int len); // 读取到串口数据后的后置处理函数（蓝牙数据 -> message）
    int (*pre_write)(char *data, int len); // 向串口中写入数据前的预处理函数（message -> 蓝牙数据）
} Device;

/**
 * @brief   初始化设备
 * @param   filename 串口文件名
 * @return  返回设备结构体指针
 */
Device *app_device_init(char *filename);

/**
 * @brief   启动设备
 * @return  返回0表示成功，返回-1表示失败
 */
int app_device_start();

/**
 * @brief   关闭设备
 */
void app_device_close();

/**
 * @brief 刷新让设置属性立即生效
 * @param device 设备结构体指针
 * @return  返回0表示成功，返回-1表示失败
 */
int app_serial_flush(Device *device);

#endif // __APP_DEVICE_H__