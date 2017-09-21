#include "createThread.h"

extern pthread_mutex_t MUTEX;
extern queue<CThostFtdcDepthMarketDataField> md_queue;

createThread::createThread()
{
  cout<<"init the thread"<<endl;
}


bool createThread::exceptionHandling(CThostFtdcDepthMarketDataField *temp)
{
  //交易日
  if (strlen(temp->TradingDay)  !=  8)
  {
    return false;
  }
    
  if (strlen(temp->UpdateTime)  !=8)
  {
    return false;

  }
    
  //各类价格
  if(temp->LastPrice  >=  DBL_MAX)
    temp->LastPrice=0;

  if(temp->PreSettlementPrice >=  DBL_MAX)
    temp->PreSettlementPrice=0;

  if(temp->PreClosePrice  >=  DBL_MAX)
    temp->PreClosePrice=0;

  if(temp->PreOpenInterest  >=  DBL_MAX)
    temp->PreOpenInterest=0;

  if(temp->OpenPrice  >=  DBL_MAX)
    temp->OpenPrice=0;

  if(temp->HighestPrice >=  DBL_MAX)
    temp->HighestPrice=0;

  if(temp->LowestPrice  >=  DBL_MAX)
    temp->LowestPrice=0;

  if(temp->Turnover >=  DBL_MAX)
    temp->Turnover=0;

  if(temp->OpenInterest >=  DBL_MAX)
    temp->OpenInterest=0;

  if(temp->ClosePrice >=  DBL_MAX)
    temp->ClosePrice=0;

  if(temp->SettlementPrice  >=  DBL_MAX)
    temp->SettlementPrice=0;

  if(temp->UpperLimitPrice  >=  DBL_MAX)
    temp->UpperLimitPrice=0;

  if(temp->LowerLimitPrice  >=  DBL_MAX)
    temp->LowerLimitPrice=0;

  if(temp->PreDelta >=  DBL_MAX)
    temp->PreDelta=0;

  if(temp->CurrDelta  >=  DBL_MAX)
    temp->CurrDelta=0;


  //申报买价与卖价
  if(temp->BidPrice1  >=  DBL_MAX)
    temp->BidPrice1=0;
  if(temp->BidPrice2  >=  DBL_MAX)
    temp->BidPrice2=0;
  if(temp->BidPrice3  >=  DBL_MAX)
    temp->BidPrice3=0;
  if(temp->BidPrice4  >=  DBL_MAX)
    temp->BidPrice4=0;
  if(temp->BidPrice5  >=  DBL_MAX)
    temp->BidPrice5=0;
  if(temp->AskPrice1  >=  DBL_MAX)
    temp->AskPrice1=0;
  if(temp->AskPrice2  >=  DBL_MAX)
    temp->AskPrice2=0;
  if(temp->AskPrice3  >=  DBL_MAX)
    temp->AskPrice3=0;
  if(temp->AskPrice4  >=  DBL_MAX)
    temp->AskPrice4=0;
  if(temp->AskPrice5  >=  DBL_MAX)
    temp->AskPrice5=0;

  return true;
}
//线程构造函数　
void* createThread::threadFunc(void *arg){
  //传入的是对象自身的一个指针
  createThread *obj = (createThread*)arg;
  obj->threadRun();
}

int createThread::threadRun(){
  //设置数据库连接串 
  //hcdbi_set_db_param( db_username, db_password, db_tnsname, ORACAL );
  //初始化连接串
  //bool connectFlag = hcdbi_init_db();
  //if ( !connectFlag )
  //{
  //  return;
  //}
  while(1)
  {
    pthread_mutex_lock(&MUTEX);
    if (md_queue.empty())
    {
      pthread_mutex_unlock(&MUTEX);
      continue;
    }
    CThostFtdcDepthMarketDataField temp=md_queue.front();
    md_queue.pop();
    pthread_mutex_unlock(&MUTEX);
    bool rightMD=exceptionHandling(&temp);
    if (!rightMD)
    {
      continue;
    }
    //updateID++;

    //hcdbi_insert_mdTick(&temp,updateID);
  }
}

 //类中创建线程的函数，如果启动成功返回０　否则返回-1
int createThread::start(){
  cout<<"start to create thread of "<<endl;
   int ret = pthread_create(&pid,NULL,threadFunc,(void *)this);
   if (ret==0){
   	 return 1;
   }
   else{
   	return 0;
   }
}

createThread::~createThread()
{
    //dtor
}
