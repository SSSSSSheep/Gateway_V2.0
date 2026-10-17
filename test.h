#if !defined(__TEST_H__)
#define __TEST_H__

typedef struct
{
    int (*task_func)(void *arg);
    void *arg;
} Task;

int test_init(int size);

void test_destory();

int test_add_task(int (task_func)(void *arg), void *arg);

#endif /* __TEST_H__ */