#include "cJSON/cJSON.h"
#include "log/log.h"
#include <stdlib.h>

int main()
{
    // 1. 生成json字符串
    cJSON *root_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(root_obj, "name", "zhangsan");
    cJSON_AddNumberToObject(root_obj, "age", 18);
    char *json = cJSON_PrintUnformatted(root_obj);
    log_debug("json: %s", json);

    // 2. 解析json字符串
    cJSON *root = cJSON_Parse(json);
    if (root == NULL)
    {
        log_error("cJSON_Parse failed");
        return -1;
    }

    char *name = cJSON_GetObjectItem(root, "name")->valuestring;
    int age = cJSON_GetObjectItem(root, "age")->valueint;
    log_debug("name: %s, age: %d", name, age);

    // 3. 释放内存
    cJSON_Delete(root_obj);
    cJSON_Delete(root);

    free(json);
    return 0;
}