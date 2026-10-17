#if !defined(__APP_BT_H__)
#define __APP_BT_H__

#include "app_device.h"

typedef enum
{
    BT_BR_9600 = '4',
    BT_BR_115200 = '8'
} BT_BaudRate;

/**
 * @brief 蓝牙模块初始化
 * @param device 设备
 * @return int 成功返回0 失败返回-1
 */
int app_bt_init(Device *device);

/**
 * @brief 蓝牙数据的写前处理函数
 * @param data 数据
 * @param data_len 数据长度
 * @return int 成功返回0 失败返回-1
 */
int app_bt_preWrite(char *data, int data_len);

/**
 * @brief 蓝牙数据的读后处理函数
 * @param data 数据
 * @param data_len 数据长度
 * @return int 成功返回0 失败返回-1
 */
int app_bt_postRead(char *data, int data_len);

/**
 * @brief 获取蓝牙状态
 * @param device 设备
 * @return int 成功返回0 失败返回-1
 */
int app_bt_status(Device *device);

/**
 * @brief 重命名蓝牙设备
 * @param device 设备
 * @param name 新名称
 * @return int 成功返回0 失败返回-1s
 */
int app_bt_rename(Device *device, char *name);

/**
 * @brief 设置蓝牙波特率
 * @param device 设备
 * @param baudRate 波特率
 * @return int 成功返回0 失败返回-1
 */
int app_bt_setBaudRate(Device *device, BT_BaudRate baudRate);

/**
 * @brief 重置蓝牙设备
 * @note 重置后需要重新初始化蓝牙设备
 * @param device 设备
 * @return int 成功返回0 失败返回-1
 */
int app_bt_reset(Device *device);

/**
 * @brief 设置蓝牙网络ID
 * @param device 设备
 * @param netId 网络ID
 * @return int 成功返回0 失败返回-1
 */
int app_bt_setNetId(Device *device, char *netid);

/**
 * @brief 设置蓝牙主地址
 * @param device 设备
 * @param maddr 主地址
 * @return int 成功返回0 失败返回-1
 */
int app_bt_setMaddr(Device *device, char *maddr);

#endif // !defined(__APP_BT_H__)