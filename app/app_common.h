#if !defined(__APP_COMMON_H)
#define __APP_COMMON_H

#include <stddef.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief 获取当前时间戳 (毫秒)
 * @return long 当前时间戳 (毫秒)
 */

long app_common_getCurrentTime();

/**
 * @brief 字符数组转16进制
 * @param chars 字符数组
 * @param chars_len 字符数组长度
 * @return char* 16进制字符串
 * 
 */


char *app_common_chars2Hexstr(const char *chars, size_t chars_len);

/**
 * @brief 16进制转字符数组
 * @param hexstr 16进制字符串
 * @param chars_len 接收字符数组长度的指针
 * @return char* 字符数组
 */
char *app_common_hexstr2Chars(const char *hexstr, size_t *chars_len);

#endif // DEBUG