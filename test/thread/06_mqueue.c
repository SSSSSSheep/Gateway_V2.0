#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "mqueue.h"

static char *mq_name = "/my_mq"; // /代表：/dev/mqueue目录下
static mqd_t mq_fd;              // 队列描述符   //description描述符

// first线程函数
void *first_thread_func(void *arg)
{
    sleep(1); // 等待first线程发送消息
    printf("first thread start send message\n");
    // 发送消息
    mq_send(mq_fd, "hello", 6, 0);
    return NULL;
}

// second线程函数
void *second_thread_func(void *arg)
{
    
    printf("second thread start receive message\n");
    // 接收消息
    char buf[1024];

    if (mq_receive(mq_fd, buf, sizeof buf, NULL) == -1)
    {
        perror("mq_receive");
        return NULL;
    }
    printf("receive message: %s\n", buf);
    return NULL;
}

int main()
{
    printf("main thread id: %d\n", gettid());

    // 创建消息队列
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;    // 消息队列中最大消息数
    attr.mq_msgsize = 1024; // 每条消息的最大字节数
    attr.mq_curmsgs = 0;

    mq_fd = mq_open(mq_name, O_CREAT | O_RDWR, 0644, &attr);
    if (mq_fd == -1)
    {
        perror("mq_open");
        return -1;
    }

    // 创建两个子线程
    pthread_t pt1, pt2; // 声明线程标识
    pthread_create(&pt1, NULL, first_thread_func, NULL);
    pthread_create(&pt2, NULL, second_thread_func, NULL);

    // 等待两个子线程结束
    pthread_join(pt1, NULL);
    pthread_join(pt2, NULL);

    printf("main thread end id: %d pt1=%ld,pt2=%ld\n", gettid(), pt1, pt2);

    // 关闭并删除队列
    mq_close(mq_fd);
    mq_unlink(mq_name);

    return 0;
}