#define _GNU_SOURCE
#include "app_bt.h"
#include "log/log.h"
#include "app_serial.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// 缓存读取到的蓝牙数据
static char read_buf[1024];
// 已读取数据的长度
static int read_len = 0;
// 固定的蓝牙数据头部
static unsigned char fixed_header[] = {0xf1, 0xdd};

static int init_bt(Device *device)
{
    // 初始化串口
    app_serial_init(device);

    // 设置串口为非阻塞模式
    app_serial_setBlock(device, 0);
    app_serial_flush(device);

    // 串口的波特率当前是9600
    // 如果当前蓝牙可用 说明蓝牙当前也是9600
    if (app_bt_status(device) == 0)
    {
        // 将蓝牙的波特率改成115200
        app_bt_setBaudRate(device, BT_BR_115200);
        // 重启蓝牙
        app_bt_reset(device);

        // 等待蓝牙启动完成
        sleep(2);
    }

    // 将串口的波特率改成115200
    app_serial_setBaudRate(device, BR_115200);
    app_serial_flush(device);

    if (app_bt_status(device) != 0)
    {
        log_error("蓝牙初始化失败");
        return -1;
    }

    app_bt_rename(device, "Sheep");  // 设置蓝牙名称
    app_bt_setNetId(device, "1111"); // 设置组网ID ：组内相同 组间不同
    app_bt_setMaddr(device, "0101"); // 设置MAC地址 ：组内不同 组间相同
    // 将串口设置为阻塞模式
    app_serial_setBlock(device, 1);
    app_serial_flush(device);

    log_debug("蓝牙初始化成功");
    return 0;
}

static int wait_ack(int fd)
{
    // 等待一段时间
    usleep(50 * 1000);
    // 读取数据
    char data_buf[4];
    read(fd, data_buf, 4);
    // 判断是否是OK\r\n
    if (memcmp(data_buf, "OK\r\n", 4) == -1)
    {
        log_debug("没有收到ACK指令");
        return -1;
    }

    log_debug("收到ACK指令");
    return 0;
}

// 删除缓存中指定长度的数据
static void remove_data(int len)
{
    memmove(read_buf, read_buf + len, read_len - len);
    read_len -= len;
}

int app_bt_init(Device *device)
{
    device->post_read = app_bt_postRead;
    device->pre_write = app_bt_preWrite;

    // 初始化蓝牙
    return init_bt(device);
}

/**
 * 向蓝牙写入数据前的处理
字符数组消息:
    例子：1 2 3 XX abc
    格式：conn_type id_len msg_len id msg
蓝牙发送数据格式例子：
    例子：41 54 2b 4d 45 53 48 00 ff ff 61 62 63 0d 0a
    41 54 2b 4d 45 53 48 00： AT+MESH（固定头部）
    ff ff: 对端的MADDR（如果是FFFF代表群发）
    61 62 63: 要发送的数据（不超过12字节）
    0d 0a：\r\n（固定结尾）
    AT+MESH XX abc \r\n （实际发送的数据）
 */
int app_bt_preWrite(char *data, int data_len)
{
    // 检查data的长度是否合法
    if (data_len < 6)
    {
        log_error("要处理的消息长度不能小于6");
        return -1;
    }

    // data[2] 是业务数据(msg)的长度
    int msg_len = (unsigned char)data[2];

    // 计算蓝牙数据的长度
    // AT+MESH(7) + CMD(1) + 目标ID(2) + 源地址(2) + 长度(1) + msg + \r\n(2)
    int blue_len = 7 + 1 + 2 + 2 + 1 + msg_len + 2;
    
    // 创建蓝牙数据数组容器
    char blue_data[blue_len];

    // 根据data中的数据组装蓝牙数据
    // data: [conn_type][id_len][msg_len][id][msg]
    // blue_data: AT+MESH + CMD + ID + 源地址 + 长度 + msg + \r\n
    
    // 1. 拷贝 AT+MESH (7字节)
    memcpy(blue_data, "AT+MESH", 7);
    
    // 2. 写入 CMD 功能码 00
    blue_data[7] = 0x00;
    
    // 3. 拷贝 id (目标地址，从 data+3 开始取2字节)
    memcpy(blue_data + 8, data + 3, 2);
    
    // 4. 写入网关源地址 01 01
    blue_data[10] = 0x01;
    blue_data[11] = 0x01;
    
    // 5. 写入业务数据长度
    blue_data[12] = (unsigned char)msg_len;
    
    // 6. 拷贝 msg (业务数据，从 data+5 开始)
    memcpy(blue_data + 13, data + 5, msg_len);
    
    // 7. 拷贝 \r\n 结尾
    memcpy(blue_data + 13 + msg_len, "\r\n", 2);

    // 清空data中的数据
    memset(data, 0, data_len);

    // 将蓝牙数据拷贝到data中
    memcpy(data, blue_data, blue_len);

    // 返回蓝牙数据长度
    return blue_len;
}

/* int app_bt_postRead(char *data, int data_len)
{
    // 将当前数据添加到缓存中
    memcpy(read_buf + read_len, data, data_len);
    read_len += data_len;

    // 如果当前已读数据的长度小于8 则继续读取 返回0
    if (read_len < 8)
    {
        log_debug("当前已读的蓝牙数据不完整(小于8字节)，继续读取");
        return 0;
    }

    // 遍历查找完整的蓝牙数据
    int i;
    for (i = 0; i < read_len - 7; i++)
    {
        // 查找开头fix_header
        if (memcmp(read_buf + i, fixed_header, 2) == 0)
        {
            // i之前都是无效数据 删除
            if (i > 0)
            {
                remove_data(i);
                // 如果缓存数据长度 小于8，继续读取
                if (read_len < 8)
                {
                    log_debug("当前已读的蓝牙数据不完整2(小于8字节)，继续读取");
                    return 0;
                }
            }

            // 有8位不代表当前这个蓝牙数据是完整的
            int blue_len = 3 + read_buf[2];
            if (read_len < blue_len)
            {
                log_debug("当前已读的蓝牙数据不完整，继续读取");
                return 0;
            }

            // 根据缓存中的蓝牙数据生成字符数组消息
            memset(data, 0, data_len);
            // conn_type
            data[0] = 1;
            // id_len
            data[1] = 2;
            // msg_len
            data[2] = blue_len - 7;
            // id
            memcpy(data + 3, read_buf + 3, 2);
            // msg
            memcpy(data + 5, read_buf + 5, data[2]);

            // 删除缓存中已处理的数据
            remove_data(blue_len);

            // 返回消息的长度
            return data[2] + 5;
        }
    }

    // 遍历的数据都是无效数据 删除
    remove_data(i);

    return 0;
} */

int app_bt_postRead(char *data, int data_len)
{
    log_debug("[postRead] ===== 进入函数 =====");
    log_debug("[postRead] 本次输入长度: %d, 当前缓存长度: %d",
              data_len, read_len);

    if (data == NULL || data_len <= 0)
    {
        log_error("[postRead] 输入参数无效");
        return 0;
    }

    // 打印本次从设备文件读取的原始字节
    printf("[postRead] 原始数据 HEX: ");
    for (int i = 0; i < data_len; i++)
    {
        printf("%02X ", (unsigned char)data[i]);
    }
    printf("\n");

    // 检查缓存容量，避免 memcpy 越界
    if (data_len > (int)sizeof(read_buf) - read_len)
    {
        log_error("[postRead] 缓存空间不足: read_len=%d, input=%d, capacity=%zu",
                  read_len, data_len, sizeof(read_buf));
        return 0;
    }

    memcpy(read_buf + read_len, data, data_len);
    read_len += data_len;

    log_debug("[postRead] 追加后缓存长度: %d", read_len);

    printf("[postRead] 当前缓存 HEX: ");
    for (int i = 0; i < read_len; i++)
    {
        printf("%02X ", (unsigned char)read_buf[i]);
    }
    printf("\n");

    if (read_len < 8)
    {
        log_debug("[postRead] 缓存不足 8 字节，等待后续数据");
        return 0;
    }

    int i;
    for (i = 0; i < read_len - 7; i++)
    {
        log_debug("[postRead] 检查帧头位置 i=%d: %02X %02X",
                  i,
                  (unsigned char)read_buf[i],
                  (unsigned char)read_buf[i + 1]);

        if (memcmp(read_buf + i, fixed_header, 2) == 0)
        {
            log_info("[postRead] 找到帧头，位置 i=%d", i);

            if (i > 0)
            {
                log_info("[postRead] 丢弃帧头前的 %d 个字节", i);
                remove_data(i);

                if (read_len < 8)
                {
                    log_debug("[postRead] 移除无效数据后，剩余长度不足 8");
                    return 0;
                }
            }

            int blue_len = 3 + (unsigned char)read_buf[2];

            log_info("[postRead] 解析长度字段: byte[2]=%02X, blue_len=%d",
                     (unsigned char)read_buf[2], blue_len);

            if (read_len < blue_len)
            {
                log_debug("[postRead] 数据帧不完整: 当前缓存=%d, 需要=%d",
                          read_len, blue_len);
                return 0;
            }

            memset(data, 0, data_len);

            data[0] = 1;
            data[1] = 2;
            data[2] = blue_len - 7;

            log_debug("[postRead] 计算消息长度: %d", (unsigned char)data[2]);

            memcpy(data + 3, read_buf + 3, 2);
            memcpy(data + 5, read_buf + 7, (unsigned char)data[2]);

            int output_len = (unsigned char)data[2] + 5;

            printf("[postRead] 输出消息 HEX: ");
            for (int j = 0; j < output_len; j++)
            {
                printf("%02X ", (unsigned char)data[j]);
            }
            printf("\n");

            remove_data(blue_len);

            log_info("[postRead] 解析成功，返回长度=%d，剩余缓存=%d",
                     output_len, read_len);

            return data[2] + 5;
        }
    }

    log_warn("[postRead] 没找到帧头 F1 DD，扫描结束位置 i=%d", i);

    // 保持原逻辑：移除扫描过的数据
    remove_data(i);

    log_debug("[postRead] 移除后剩余缓存长度=%d", read_len);

    return 0;
}

int app_bt_status(Device *device)
{
    // 向蓝牙串口文件中写入"AT\r\n"的指令数据
    write(device->fd, "AT\r\n", 4);
    // 通过读取"OK\r\n"数据判断蓝牙是否可用
    return wait_ack(device->fd);
}

int app_bt_rename(Device *device, char *name)
{
    // 拼接AT指令
    char cmd[64];
    sprintf(cmd, "AT+NAME%s\r\n", name);
    // 写入指令
    write(device->fd, cmd, strlen(cmd));
    // 等待ACK
    return wait_ack(device->fd);
}

int app_bt_setBaudRate(Device *device, BT_BaudRate baudRate)
{
    // 拼接AT指令
    char cmd[64];
    sprintf(cmd, "AT+BAUD%c\r\n", baudRate);
    // 写入指令
    write(device->fd, cmd, strlen(cmd));
    return wait_ack(device->fd);
}

int app_bt_reset(Device *device)
{
    // AT指令
    char *cmd = "AT+RESET\r\n";
    // 写入指令
    write(device->fd, cmd, strlen(cmd));
    return wait_ack(device->fd);
}

int app_bt_setNetId(Device *device, char *netid)
{
    // 拼接AT指令
    char cmd[64];
    sprintf(cmd, "AT+NETID%s\r\n", netid);
    // 写入指令
    write(device->fd, cmd, strlen(cmd));
    return wait_ack(device->fd);
}

int app_bt_setMaddr(Device *device, char *maddr)
{
    // 拼接AT指令
    char cmd[64];
    sprintf(cmd, "AT+MADDR%s\r\n", maddr);
    // 写入指令
    write(device->fd, cmd, strlen(cmd));
    return wait_ack(device->fd);
}
