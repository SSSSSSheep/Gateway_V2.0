#include "app_mqtt.h"
#include "log/log.h"
#include <unistd.h>

int mqtt_recv_callback(const char *json)
{
    log_debug("mqtt_recv_callback: %s", json);
    return 0;
}

int main(int argc, char *argv[])
{
    app_mqtt_init();

    app_mqtt_registerRecvCallback(mqtt_recv_callback);

    app_mqtt_send("{\"conn_type\":1,\"id\":\"5858\",\"msg\":\"61626364\"}");

    sleep(80);

    app_mqtt_close();

    return 0;
}