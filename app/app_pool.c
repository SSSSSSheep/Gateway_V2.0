#include "app_pool.h"
#include "log/log.h"
#include <pthread.h>
#include <stdlib.h>
#include <mqueue.h>

// 线程数量
static int thread_nums;
// 线程池中所有线程标识的容器
static pthread_t *thread_pool;
// 消息队列标识
static mqd_t mq_fd;
// 消息队列名称
static char *mq_name = "/app_pool_mq";

void *thread_fun(void *arg)
{
    Task task;
    while (1)
    {
        // 从任务队列中获取任务并执行
        int len = mq_receive(mq_fd, (char *)&task, sizeof(Task), NULL);
        // 执行任务
        if (len == sizeof(Task))
        {
            task.task_fun(task.arg);
        }
    }
}

int app_pool_init(int size)
{
    // 初始化任务（消息）队列
    struct mq_attr attr;
    // 消息队列中最大消息数量
    attr.mq_maxmsg = 10;
    // 每个消息的最大字节数
    attr.mq_msgsize = sizeof(Task);
    mq_fd = mq_open(mq_name, O_CREAT | O_RDWR, 0644, &attr);
    if (mq_fd == -1)
    {
        log_error("创建消息队列失败");
        return -1;
    }

    // 初始化线程池
    thread_nums = size;
    thread_pool = malloc(sizeof(pthread_t) * size);
    for (int i = 0; i < size; i++)
    {
        pthread_create(&thread_pool[i], NULL, thread_fun, NULL);
    }

    log_debug("线程池初始化成功");
    return 0;
}

void app_pool_destroy()
{
    // 关闭消息队列
    mq_close(mq_fd);
    mq_unlink(mq_name);
    // 销毁线程池
    for (int i = 0; i < thread_nums; i++)
    {
        pthread_cancel(thread_pool[i]);
        pthread_join(thread_pool[i], NULL);
    }
    free(thread_pool);
}

int app_pool_addTask(int (*task_fun)(void *arg), void *arg)
{
    // 创建任务
    Task task = {
        .task_fun = task_fun,
        .arg = arg,
    };
    // 将任务发送到消息队列中
    return mq_send(mq_fd, (char *)&task, sizeof(Task), 1);
}
