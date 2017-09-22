#ifndef CREATETHREAD_H
#define CREATETHREAD_H
#include "../../include/ThostFtdcUserApiStruct.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <float.h>
#include <queue>
#include <occi.h>
using namespace std;
using namespace oracle::occi;

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
        string createSQL(CThostFtdcDepthMarketDataField *temp);
        bool exceptionHandling(CThostFtdcDepthMarketDataField *temp);

        Environment *env;
        Connection *conn;
        Statement *pStmt;
        string oracle_name;
        string oracle_pass;
};

#endif // CREATETHREAD_H
