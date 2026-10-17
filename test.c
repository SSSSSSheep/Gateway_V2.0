#include "test.h"
#include <pthread.h>
#include <stdio.h>
#include <mqueue.h>

static int task_num;
static pthread_t *thread_pool;

static mqd_t mq_fd;
static char *mq_name = "/test_mq";

void *thread_func(void *arg)
{
    Task task;
    while (1)
    {
        int len = mq_receive(mq_fd, (char *)&task, sizeof(Task), NULL);
        if (len == sizeof(Task))
            task.task_func(task.arg);
    }
}

int test_init(int size)
{
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Task);

    mq_fd = mq_open(mq_name, O_RDWR | O_CREAT, 0644, &attr);
    if (mq_fd == -1)
    {
        printf("mq open error");
        return -1;
    }

    task_num = size;
    thread_pool = (pthread_t *)malloc(sizeof(pthread_t) * size);
    if (!thread_pool)
    {
        printf("malloc error");
        return -1;
    }
    for (int i = 0; i < size; i++)
    {
        pthread_create(&thread_pool[i], NULL, thread_func, NULL);
    }
    return 0;
}

void test_destory()
{
    mq_close(mq_fd);
    mq_unlink(mq_name);

    for (int i = 0; i < task_num; i++)
    {
        pthread_cancel(thread_pool[i]);
        pthread_join(thread_pool[i], NULL);
        }
    free(thread_pool);
}

int test_add_task(int(task_func)(void *arg), void *arg)
{

    Task task = {
        .task_func = task_func,
        .arg = arg};
    mq_send(mq_fd, (char *)&task, sizeof(Task), 1);
}