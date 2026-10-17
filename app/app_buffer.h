#if !defined(__APP_BUFFER_H__)
#define __APP_BUFFER_H__
#include <pthread.h>

// 内部的子缓冲区
typedef struct
{
    unsigned char *ptr;      // 指向缓冲区的指针
    int total_size; // 缓冲区大小
    int len;        // 当前缓冲区已使用的长度
} SubBuffer;

typedef struct
{
    SubBuffer *sub_buffers[2];  // 两个子缓冲区
    int read_index;             // 当前读取的子缓冲区索引
    int write_index;            // 当前写入的子缓冲区索引
    pthread_mutex_t read_lock;  // 读锁
    pthread_mutex_t write_lock; // 写锁
} Buffer;

/**
 * @brief 初始化缓冲区
 * @param size 缓冲区大小
 * @return Buffer* 返回缓冲区指针
 */
Buffer *app_buffer_init(int size);

/**
 * @brief 释放缓冲区
 * @param buffer 缓冲区指针
 * @return void
 */
void app_buffer_free(Buffer *buffer);

/**
 * @brief 写入数据到缓冲区
 * @param buffer 缓冲区指针
 * @param data 数据指针
 * @param len 数据长度
 * @return int 返回写入的字节数
 */
int app_buffer_write(Buffer *buffer, const char *data, int data_len);

/**
 * @brief 从缓冲区读取数据
 * @param buffer 缓冲区指针
 * @param data 数据指针
 * @param data_len 数据长度
 * @return int 返回读取的字节数
 */
int app_buffer_read(Buffer *buffer, char *data_buf, int data_size);

#endif // __APP_BUFFER_H__