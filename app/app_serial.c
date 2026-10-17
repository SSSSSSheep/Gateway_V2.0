#define _GNU_SOURCE
#include "app_serial.h"
#include "log/log.h"

int app_serial_setBaudRate(Device *device, BaudRate baudRate)
{
    // 读取串口属性
    struct termios attr;
    tcgetattr(device->fd, &attr);
    // 修改属性（波特率）
    cfsetspeed(&attr, baudRate);
    // 设置串口属性（当前不生效 都设置好后flush才生效）
    int ret = tcsetattr(device->fd, TCSAFLUSH, &attr);
    if (ret == -1)
    {
        log_error("设置串口波特率失败");
        return -1;
    }
    log_info("设置串口波特率成功");
    return 0;
}

int app_serial_setParty(Device *device, Parity parity)
{
    // 读取串口属性
    struct termios attr;
    tcgetattr(device->fd, &attr);
    // 修改属性（校验位）
    attr.c_cflag &= ~(PARENB | PARODD); // 清除校验位
    attr.c_cflag |= parity;

    // 设置串口属性（当前不生效 都设置好后flush才生效）
    int ret = tcsetattr(device->fd, TCSAFLUSH, &attr);
    if (ret == -1)
    {
        log_error("设置串口校验位失败");
        return -1;
    }
    log_info("设置串口校验位成功");
    return 0;
}

int app_serial_setStopBit(Device *device, StopBit stopBit)
{
    // 读取串口属性
    struct termios attr;
    tcgetattr(device->fd, &attr);
    // 修改属性（停止位）
    attr.c_cflag &= ~CSTOPB; // 清除停止位
    attr.c_cflag |= stopBit;
    // 设置串口属性（当前不生效 都设置好后flush才生效）
    int ret = tcsetattr(device->fd, TCSAFLUSH, &attr);
    if (ret == -1)
    {
        log_error("设置串口停止位失败");
        return -1;
    }
    log_info("设置串口停止位成功");
    return 0;
}

int app_serial_setBlock(Device *device, int is_block)
{
    // 读取串口属性
    struct termios attr;
    tcgetattr(device->fd, &attr);
    // 修改属性（阻塞模式）
    if (is_block)
    {
        // 阻塞模式
        attr.c_cc[VMIN] = 1;  // 至少读取一个字节才返回 -> 没有读取到数据不返回
        attr.c_cc[VTIME] = 0; // 读取超时时间 -> 不超时
    }
    else
    {
        // 非阻塞模式
        attr.c_cc[VMIN] = 0;  // 最少读取的字符数是0 -> 没有读取到数据也返回
        attr.c_cc[VTIME] = 2; // 读取超时时间 -> 单位是100ms 0.2s
    }
    // 设置串口属性（当前不生效 都设置好后flush才生效）
    int ret = tcsetattr(device->fd, TCSAFLUSH, &attr);
    if (ret == -1)
    {
        log_error("设置串口阻塞模式失败");
        return -1;
    }
    log_info("设置串口阻塞模式成功");
    return 0;
}

int app_serial_setRaw(Device *device)
{
    // 读取串口属性
    struct termios attr;
    tcgetattr(device->fd, &attr);
    // 修改属性（原始模式）
    cfmakeraw(&attr);
    // 设置串口属性（当前不生效 都设置好后flush才生效）
    int ret = tcsetattr(device->fd, TCSAFLUSH, &attr);
    if (ret == -1)
    {
        log_error("设置串口原始模式失败");
        return -1;
    }
    log_info("设置串口原始模式成功");
    return 0;
}

int app_serial_init(Device *device)
{
    // 初始化串口的各个属性
    app_serial_setBaudRate(device, BR_9600);
    app_serial_setParty(device, Parity_NONE);
    app_serial_setStopBit(device, SB_1);
    app_serial_setBlock(device, 0);
    app_serial_setRaw(device);
    // 刷新生效
    int ret = tcflush(device->fd, TCIOFLUSH);
    if (ret == -1)
    {
        log_error("刷新串口属性失败");
        return -1;
    }
    log_debug("初始化串口成功");
    return 0;
}
