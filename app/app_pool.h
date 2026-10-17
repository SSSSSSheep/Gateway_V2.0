#if !defined(__APP_POOL_H__)
#define __APP_POOL_H__

// 任务体结构
typedef struct
{
    // 任务函数
    int (*task_fun)(void *arg);
    // 任务函数执行时需要的参数
    void *arg;
} Task;

/**
 * @function: 线程池初始化
 * @param size 线程池中线程的数量
 * @return 0:成功，-1:失败
 */
int app_pool_init(int size);

/**
 * @function: 销毁线程池
 */
void app_pool_destroy();

/**
 * @function: 添加任务到线程池
 * @param task 任务体
 * @param arg 任务参数
 * @return 0:成功，-1:失败
 */
int app_pool_addTask(int (*task_fun)(void *arg), void *arg);

#endif // !defined(__APP_POOL_H__)