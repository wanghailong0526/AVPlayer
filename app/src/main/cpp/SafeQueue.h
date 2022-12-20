//
// Created by 王海龙 on 2022/11/17.
//

#ifndef AVPLAYER_SAFEQUEUE_H
#define AVPLAYER_SAFEQUEUE_H

#include <queue>
#include <pthread.h>

using std::queue;
//using namespace std;

template<typename T>
class SafeQueue {
private:
    typedef void (*ReleaseCallback)(T *);//定义函数指针 由外界释放 T 里的内容

public:
    queue<T> queue;
    int work = 0;//队列是否工作 默认不工作
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    ReleaseCallback releaseCallback;

    SafeQueue() {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&cond, nullptr);
    }

    ~SafeQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    bool empty() {
        return queue.empty();
    }

    int size() {
        return queue.size();
    }

    //数据插入队列 AVPacket*_压缩包 AVFrame*_原始包
    void push(T v) {
        pthread_mutex_lock(&mutex);

        if (work) {
            queue.push(v);//入队列
            pthread_cond_signal(&cond);//通知取数据
        } else {//非工作状态通过外界释放 value
            if (releaseCallback) {
                releaseCallback(&v);
            }
        }

        pthread_mutex_unlock(&mutex);
    }

    //从队列里取数据
    int pop(T &v) {
        int r = 0;

        pthread_mutex_lock(&mutex);

        //如果不是工作状态或队列是空的就阻塞
        while (!work || empty()) {
            pthread_cond_wait(&cond, &mutex);
        }
        if (!empty()) {
            v = queue.front();
            queue.pop();//删除队列中刚刚取出的数据
            r = 1;
        }

        pthread_mutex_unlock(&mutex);
        return r;
    }

    /**
     * 设置队列是否工作
     * @param work
     */
    void setwork(int work) {
        pthread_mutex_lock(&mutex);

        this->work = work;
        //每次设置状态后都要唤醒一下，避免有阻塞的地方
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);
    }

    /**
     * 清空队列
     */
    void clear() {
        pthread_mutex_lock(&mutex);

        while (!empty()) {
            T v = queue.front();
            if (!releaseCallback) {
                releaseCallback(&v);
            }
            queue.pop();
        }

        pthread_mutex_unlock(&mutex);
    }

    /**
     * 设置回调，让外界释放 value
     * @param releaseCallback
     */
    void setReleaseCallback(ReleaseCallback releaseCallback) {
        this->releaseCallback = releaseCallback;
    }


};

#endif //AVPLAYER_SAFEQUEUE_H