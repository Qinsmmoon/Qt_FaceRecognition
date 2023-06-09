// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#ifndef NCNN_PLATFORM_H
#define NCNN_PLATFORM_H

#define NCNN_STDIO 1
#define NCNN_STRING 1
#define NCNN_OPENCV 0
#define NCNN_BENCHMARK 0
#define NCNN_PIXEL 1
#define NCNN_PIXEL_ROTATE 0
#define NCNN_VULKAN 0
#define NCNN_REQUANT 0
#define NCNN_AVX2 0

#include <pthread.h>

namespace ncnn {


class Mutex
{
public:
    Mutex() { pthread_mutex_init(&mutex, 0); }
    ~Mutex() { pthread_mutex_destroy(&mutex); }
    void lock() { pthread_mutex_lock(&mutex); }
    void unlock() { pthread_mutex_unlock(&mutex); }
private:
    friend class ConditionVariable;
    pthread_mutex_t mutex;
};

class MutexLockGuard
{
public:
    MutexLockGuard(Mutex& _mutex) : mutex(_mutex) { mutex.lock(); }
    ~MutexLockGuard() { mutex.unlock(); }
private:
    Mutex& mutex;
};


class ConditionVariable
{
public:
    ConditionVariable() { pthread_cond_init(&cond, 0); }
    ~ConditionVariable() { pthread_cond_destroy(&cond); }
    void wait(Mutex& mutex) { pthread_cond_wait(&cond, &mutex.mutex); }
    void broadcast() { pthread_cond_broadcast(&cond); }
    void signal() { pthread_cond_signal(&cond); }
private:
    pthread_cond_t cond;
};


class Thread
{
public:
    Thread(void* (*start)(void*), void* args = 0) { pthread_create(&t, 0, start, args); }
    ~Thread() {}
    void join() { pthread_join(t, 0); }
private:
    pthread_t t;
};

} // namespace ncnn

#endif // NCNN_PLATFORM_H
