#include "app_message.h"
#include "log/log.h"

int main()
{
    char *json_msg = "{\"conn_type\":1,\"id\":\"5858\",\"msg\":\"61626364\"}";

    // json 消息转字符数组消息
    char chars_buf[100];
    int len = app_message_json2chars(json_msg, chars_buf, sizeof(chars_buf));
    log_debug("字符数组消息：%.*s,长度：%d", len, chars_buf, len);

    // 字符数组消息 转 json消息
    char *json_msg2 = app_message_chars2json(chars_buf, len);
    log_debug("json消息：%s", json_msg2);

    free(json_msg2);

    return 0;
}
