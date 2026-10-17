#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// first线程函数
void *first_thread_func(void *arg)
{
    printf("first thread id: %d\n", gettid());
    return NULL;
}

// second线程函数
void *second_thread_func(void *arg)
{
    printf("second thread id: %d\n", gettid());
    return NULL;
}

int main()
{
    printf("main thread id: %d\n", gettid());

    // 创建两个子线程
    pthread_t pt1, pt2; // 声明线程标识
    pthread_create(&pt1, NULL, first_thread_func, NULL);
    pthread_create(&pt2, NULL, second_thread_func, NULL);

    // 等待两个子线程结束
    pthread_join(pt1, NULL);
    pthread_join(pt2, NULL);

    printf("main thread end id: %d pt1=%ld,pt2=%ld\n", gettid(), pt1, pt2);

    return 0;
}