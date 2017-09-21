#ifndef CREATETHREAD_H
#define CREATETHREAD_H
#include "../api/ThostFtdcUserApiStruct.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <float.h>
#include <queue>
using namespace std;

class createThread
{
    public:
        createThread();
        //线程启动函数
         int start();
         //静态函数，线程函数
         static void* threadFunc(void *arg);
         virtual ~createThread();
    protected:
    private:
        //线程执行函数
         pthread_t pid;
        int threadRun();
        bool exceptionHandling(CThostFtdcDepthMarketDataField *temp);
};

#endif // CREATETHREAD_H
