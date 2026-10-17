#if !defined(__APP_MESSAGE_H__)
#define __APP_MESSAGE_H__

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON/cJSON.h"
#include "log/log.h"
#include "app_message.h"
#include "app_common.h"

/**
 * @brief   字符数组消息转json消息
 * @param   chars       字符数组
 * @param   chars_len   字符数组长度
 * @return  json消息
 */
char *app_message_chars2json(char *chars, int chars_len);

/**
 * @brief   json消息转字符数组消息
 * @param   json        json消息
 * @param   chars_buf   字符数组缓冲区
 * @param   buf_size    字符数组缓冲区大小
 * @return  字符数组消息长度
 */
int app_message_json2chars(char *json, char *chars_buf, int buf_size);

#endif /* !__APP_MESSAGE_H__ */