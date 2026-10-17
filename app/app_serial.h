#if !defined(__APP_SERIAL_H__)
#define __APP_SERIAL_H__

#include <termios.h>
#include "app_device.h"

// 波特率
typedef enum
{
    BR_9600 = B9600,
    BR_115200 = B115200,
} BaudRate;

// 校验位
typedef enum
{
    Parity_NONE = 0,
    Parity_ODD = PARENB | PARODD,
    Parity_EVEN = PARENB,
} Parity;

// 停止位
typedef enum
{
    SB_1 = 0,
    SB_2 = CSTOPB,
} StopBit;

/**
 * @brief 初始化串口
 * @param device 设备
 * @return 0 成功，-1 失败
 */
int app_serial_init(Device *device);

/**
 * @brief 设置串口波特率
 * @param device 设备
 * @param baudRate 波特率
 * @return 0 成功，-1 失败
 */
int app_serial_setBaudRate(Device *device, BaudRate baudRate);

/**
 * @brief 设置串口校验位
 * @param device 设备
 * @param parity 校验位
 * @return 0 成功，-1 失败
 */
int app_serial_setParty(Device *device, Parity parity);

/**
 * @brief 设置串口停止位
 * @param device 设备
 * @param stopBit 停止位
 * @return 0 成功，-1 失败
 */
int app_serial_setStopBit(Device *device, StopBit stopBit);

/**
 * @brief 设置串口阻塞模式
 * @param device 设备
 * @param is_block 是否阻塞，1 阻塞，0 非阻塞
 * @return 0 成功，-1 失败
 */
int app_serial_setBlock(Device *device, int is_block);

/**
 * @brief 设置串口为原始模式
 * @param device 设备
 * @return 0 成功，-1 失败
 */
int app_serial_setRaw(Device *device);

#endif // __APP_SERIAL_H__