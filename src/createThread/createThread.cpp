#include "createThread.h"

extern pthread_mutex_t MUTEX;
extern queue<CThostFtdcDepthMarketDataField> md_queue;

createThread::createThread()
{
  cout<<"start init the thread"<<endl;
  updateid =1;
  oracle_name = "hyqh";
  oracle_pass = "hyqh";
  env=Environment::createEnvironment();
  // conn = env->createConnection(oracle_name,oracle_pass);
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
  while(1){
          try
          {
            // Connection *conn = env->createConnection(name,pass,srvName);
            conn = env->createConnection(oracle_name,oracle_pass);
            cout<<"conn success"<<endl;

        //  数据操作,创建Statement对象
            pStmt = NULL;    // Statement对象
            pStmt = conn->createStatement();
            if(NULL == pStmt) {
              cout<<"create statement error"<<endl;
              return -1;
            }
                   

            cout<<"has init the Statement\n"<<endl;
            //--------插入---------
            // 指定DML为自动提交
            pStmt->setAutoCommit(TRUE);
          }
          catch(SQLException e){
            cout<<"catch a error"<<endl;
            cout<<e.what()<<endl;
            try{
              conn->terminateStatement(pStmt);
            }
            catch(exception& e){
             
            }
          }

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
            //start insert to database
            std::string sql = createSQL(&temp);
           // cout<<sql<<endl;
            //pStmt->setSQL("INSERT INTO TA (ID, NAME) VALUES (1, 'ZS')");
            try{
              pStmt->setSQL(sql);

              // 执行SQL语句
              unsigned int nRet = pStmt->executeUpdate();

              if(nRet == 0) {
                  cout<<"executeUpdate insert error.\n"<<endl;
              }
            }
            catch(SQLException e){
            	cout<<"catch a error"<<endl;
            	cout<<e.what()<<endl;
            	cout<<sql<<endl;
            }

          }

          conn->terminateStatement(pStmt);
          env->terminateConnection(conn);

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

string createThread::createSQL(CThostFtdcDepthMarketDataField *temp){
  char ret[2048]={0};

  sprintf(ret,"INSERT INTO TEST (TRADINGDAY,INSTRUMENTID,EXCHANGEID,EXCHANGEINSTID,LASTPRICE,PRESETTLEMENTPRICE,PRECLOSEPRICE,PREOPENINTEREST,OPENPRICE,HIGHESTPRICE,LOWESTPRICE,VOLUME,TURNOVER,OPENINTEREST,CLOSEPRICE, SETTLEMENTPRICE,UPPERLIMITPRICE,LOWERLIMITPRICE,PREDELTA,CURRDELTA,UPDATETIME,UPDATEMILLISEC,BIDPRICE1,BIDVOLUME1,ASKPRICE1,ASKVOLUME1,BIDPRICE2,BIDVOLUME2,ASKPRICE2,ASKVOLUME2,BIDPRICE3,BIDVOLUME3,ASKPRICE3,ASKVOLUME3,BIDPRICE4, BIDVOLUME4,ASKPRICE4,ASKVOLUME4,BIDPRICE5,BIDVOLUME5,ASKPRICE5,ASKVOLUME5,AVERAGEPRICE,UPDATEID) VALUES ('%s','%s','%s','%s',%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,'%s',%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%.0f)",
    temp->TradingDay,temp->InstrumentID,temp->ExchangeID,temp->ExchangeInstID
    ,temp->LastPrice,temp->PreSettlementPrice,temp->PreClosePrice,temp->PreOpenInterest
    ,temp->OpenPrice,temp->HighestPrice,temp->LowestPrice,temp->Volume
    ,temp->Turnover,temp->OpenInterest,temp->ClosePrice,temp->SettlementPrice
    ,temp->UpperLimitPrice,temp->LowerLimitPrice,temp->PreDelta,temp->CurrDelta
    ,temp->UpdateTime,temp->UpdateMillisec,temp->BidPrice1,temp->BidVolume1
    ,temp->AskPrice1,temp->AskVolume1,temp->BidPrice2,temp->BidVolume2
    ,temp->AskPrice2,temp->AskVolume2,temp->BidPrice3,temp->BidVolume3
    ,temp->AskPrice3,temp->AskVolume3,temp->BidPrice4,temp->BidVolume4
    ,temp->AskPrice4,temp->AskVolume4,temp->BidPrice5,temp->BidVolume5
    ,temp->AskPrice5,temp->AskVolume5,temp->AveragePrice,updateid);

  // sprintf(ret,"INSERT INTO TEST (TRADINGDAY,INSTRUMENTID,EXCHANGEID,EXCHANGEINSTID,LASTPRICE,PRESETTLEMENTPRICE,PRECLOSEPRICE,PREOPENINTEREST,OPENPRICE,HIGHESTPRICE,LOWESTPRICE,VOLUME,TURNOVER,OPENINTEREST,CLOSEPRICE, SETTLEMENTPRICE,UPPERLIMITPRICE,LOWERLIMITPRICE,PREDELTA,CURRDELTA,UPDATETIME,UPDATEMILLISEC,BIDPRICE1,BIDVOLUME1,ASKPRICE1,ASKVOLUME1,BIDPRICE2,BIDVOLUME2,ASKPRICE2,ASKVOLUME2,BIDPRICE3,BIDVOLUME3,ASKPRICE3,ASKVOLUME3,BIDPRICE4, BIDVOLUME4,ASKPRICE4,ASKVOLUME4,BIDPRICE5,BIDVOLUME5,ASKPRICE5,ASKVOLUME5,AVERAGEPRICE,UPDATEID) VALUES ('%s','%s','%s','%s',%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,'%s',%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%.0f)",
  //   temp->TradingDay,temp->InstrumentID,temp->ExchangeID,temp->ExchangeInstID
  //   ,5.0,6.0,7.0,8.0
  //   ,9.0,10.0,11.0,12
  //   ,13.0,14.0,15.0,16.0
  //   ,17.0,18.0,19.0,20.0
  //   ,temp->UpdateTime,temp->UpdateMillisec,23.0,24
  //   ,25.0,temp->AskVolume1,temp->BidPrice2,temp->BidVolume2
  //   ,temp->AskPrice2,temp->AskVolume2,temp->BidPrice3,temp->BidVolume3
  //   ,temp->AskPrice3,temp->AskVolume3,temp->BidPrice4,temp->BidVolume4
  //   ,temp->AskPrice4,temp->AskVolume4,temp->BidPrice5,temp->BidVolume5
  //   ,temp->AskPrice5,temp->AskVolume5,temp->AveragePrice,updateid);

  updateid++;
  
  return (string)ret;
}

createThread::~createThread()
{
    //dtor
  Environment::terminateEnvironment(env);
  cout<<"close the resources"<<endl;
  try{
    conn->terminateStatement(pStmt);
    env->terminateConnection(conn);
    conn->terminateStatement(pStmt);
    cout<<"release the source succed"<<endl;
  }
  catch(exception& e){
    cout<<"cant release the source"<<endl;
  }
}
