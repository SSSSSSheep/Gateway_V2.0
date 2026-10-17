#include "app_common.h"

long app_common_getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    // tv_sec: 秒 tv_usec: 微秒
}

/**
 * [abcd] -> [61, 62, 63, 64]
 */
char *app_common_chars2Hexstr(const char *chars, size_t chars_len)
{
    // 申请16进制字符串空间
    char *hexstr = (char *)malloc(chars_len * 2 + 1);

    // 遍历chars中的每个字符，将字符转化为对应的两位16进制字符保存到hexstr中
    for (size_t i = 0; i < chars_len; i++)
    {
        sprintf(hexstr + i * 2, "%02X", chars[i] & 0xff);
    }

    // 向hexstr中添加结束符
    hexstr[chars_len * 2] = '\0';

    // 返回hexstr
    return hexstr;
}

/**
 * [61, 62, 63, 64] -> [abcd]
 */
char *app_common_hexstr2Chars(const char *hexstr, size_t *chars_len)
{
    if (hexstr == NULL || chars_len == NULL)
    {
        return NULL;
    }
    
    // 得到hexstr的长度
    int hexstr_len = strlen(hexstr);
    // 保留至V2优化
    if (hexstr_len % 2 != 0)
    {
        // 非法
    }
    // 指定chars_len 的值
    *chars_len = strlen(hexstr) / 2;

    // 申请chars空间
    char *chars = (char *)malloc(*chars_len);

    // 遍历hexstr中的每2个16进制字符(以2个为单位)，得到1个对应的字符 保存到chars
    for (size_t i = 0; i < hexstr_len; i += 2)
    {
        sscanf(hexstr + i, "%02X", (unsigned int *)(chars + i / 2));
    }

    // 返回chars
    return chars;
}
