#include "app_buffer.h"
#include "log/log.h"

int main(int argc, char *argv[])
{
    Buffer *buffer0 = app_buffer_init(1000);
    char data[255];
    for (int i = 0; i < 255; i++)
    {
        data[i] ='a';
    }
    app_buffer_write(buffer0, data, 255);
    char data_buf0[300];
    int data_len0 = app_buffer_read(buffer0, data_buf0, 300);
    log_debug("读取数据0： %.*s, 长度：%d", data_len0, data_buf0, data_len0);
    

    Buffer *buffer = app_buffer_init(13);

    // 写数据1
    app_buffer_write(buffer, "abcde", 5);
    app_buffer_write(buffer, "zmjqkA", 6);

    // 读数据1
    char data_buf[10];
    int len = app_buffer_read(buffer, data_buf, 10);
    log_debug("读取数据1： %.*s, 长度：%d", len, data_buf, len);
    // 读数据2
    char data_buf2[10];
    len = app_buffer_read(buffer, data_buf2, 10);
    log_debug("读取数据2： %.*s, 长度：%d", len, data_buf2, len);
    // 读数据3
    char data_buf3[10];
    len = app_buffer_read(buffer, data_buf3, 10);
    log_debug("读取数据3： %.*s, 长度：%d", len, data_buf3, len);

    // 写数据2
    app_buffer_write(buffer, "kskblzdjdwkzkbl", 15);

    app_buffer_free(buffer);

    return 0;
}