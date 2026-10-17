#if !defined(__APP_MQTT_H__)
#define __APP_MQTT_H__

#define ADDRESS "ws://192.168.0.101:8083"
#define CLIENTID "b253ba38-daf6-4b37-984f-5d8fdc6a1cfa"
#define TOPIC_R2G "remote_to_gateway" // 接收消息的主题
#define TOPIC_G2R "gateway_to_remote" // 发送消息的主题
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L

/**
 * @brief 初始化MQTT客户端
 */
int app_mqtt_init(void);

/**
 * @brief 关闭MQTT客户端
 */
void app_mqtt_close();

/**
 * @brief 发送MQTT消息
 */
int app_mqtt_send(const char *json);

/**
 * @brief 注册MQTT消息接收回调函数
 * @param callback 回调函数，参数为接收到的JSON字符串
 */
void app_mqtt_registerRecvCallback(int (*callback)(char *json));

#endif // __APP_MQTT_H__