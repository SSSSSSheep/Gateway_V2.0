#include "app_mqtt.h"
#include "log/log.h"
#include <MQTTClient.h>
#include <string.h>

static MQTTClient_message pubmsg = MQTTClient_message_initializer;
static MQTTClient client;
static MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

static int (*recv_callback)(char *json) = NULL;

// 发送消息回调函数
static void delivered(void *context, MQTTClient_deliveryToken dt)
{
    log_debug("Message send Success!");
}

// 接收远程消息回调
// 注意: 返回1表示消息处理成功，返回0表示消息处理失败
static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int result = 0;
    if (recv_callback)
    {
        result = recv_callback((char *)message->payload) == 0 ? 1 : 0;
    }

    // 释放消息资源
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return result;
}

// 连接断开回调函数
static void connlost(void *context, char *cause)
{
    log_error("MQTT connection lost: %s", cause);
}

int app_mqtt_init(void)
{
    // 1. 初始化MQTT客户端
    if (MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_create failed");
        return -1;
    }
    // 2. 设置回调函数
    if (MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered) != MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_setCallbacks failed");
        MQTTClient_destroy(&client);
        return -1;
    }
    // 3. 连接MQTT服务器
    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_connect failed");
        MQTTClient_destroy(&client);
        return -1;
    }
    // 4. 订阅主题
    if (MQTTClient_subscribe(client, TOPIC_R2G, QOS) != MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_subscribe failed");
        MQTTClient_disconnect(client, TIMEOUT);
        MQTTClient_destroy(&client);
        return -1;
    }
    log_debug("MQTT init Success!");
    return 0;
}

void app_mqtt_close()
{
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
}

int app_mqtt_send(const char *json)
{
    // 指定要发送的数据
    pubmsg.payload = (void *)json;
    pubmsg.payloadlen = strlen(json);
    pubmsg.qos = QOS;

    if (MQTTClient_publishMessage(client, TOPIC_G2R, &pubmsg, NULL) != MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_publishMessage failed");
        return -1;
    }

    log_debug("Message send Success! %s", json);
    return 0;
}

void app_mqtt_registerRecvCallback(int (*callback)(char *json))
{
    // TODO:
    recv_callback = callback;
}
