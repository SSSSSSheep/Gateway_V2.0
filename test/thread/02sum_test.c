#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

int calc_sqr(int a)
{
    sleep(1);
    return a * a;
}

// 线程函数，计算平方值
void *calc_thread_fun(void *arg)
{
    int num = *(int *)arg;
    int *result = malloc(sizeof(int));
    *result = calc_sqr(num);
    // return result;

    pthread_exit(result);
}

int main()
{
    int arr[5] = {1, 2, 3, 4, 5};
    int sqrs[5];
    pthread_t pts[5];
    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < 5; i++)
    {
        // sqrs[i] = calc_sqr(arr[i]);
        // 创建线程进行长时间的计算
        pthread_create(&pts[i], NULL, calc_thread_fun, (void *)&arr[i]);
    }

    for (int i = 0; i < 5; i++)
    {
        void *result;
        pthread_join(pts[i], &result);
        sqrs[i] = *(int *)result;
        free(result);
    }

    gettimeofday(&end, NULL);
    printf("time: %ld秒\n", end.tv_sec - start.tv_sec);

    // 累加所有的平方值
    int sum = 0;
    for (int i = 0; i < 5; i++)
    {
        sum += sqrs[i];
    }
    printf("sum: %d\n", sum);
    return 0;
}