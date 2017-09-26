#include "./application.h"

extern pthread_mutex_t MUTEX;
extern queue<CThostFtdcDepthMarketDataField> md_queue;


application::application() {
  // ctor
	requestid_ = 1;
}

void application::ConfigFun() {
    //此函数主要是用来获取一些配置信息包括前置地址等。通过读取config.txt来获得信息。
	string path = "./config.txt";
	unordered_map<string,string> ret =  GetConfigInfo(path);
	MDFRONT=ChangeStrToChar(ret["MDFRONT"]);
	TRADEFRONT=ChangeStrToChar(ret["TRADERFRONT"]);
	strcpy(BrokerID,ChangeStrToChar(ret["BrokerID"]));
	strcpy(USERID,ChangeStrToChar(ret["USERID"]));
	strcpy(PASSWD,ChangeStrToChar(ret["PASSWD"]));


  cout<<BrokerID<<endl;
  cout<<USERID<<endl;
  cout<<PASSWD<<endl;
	
}

void application::Init() {
    //初始化MDApi
    pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
    pMdSpi = new CtpMdSpi();  //创建回调处理类对象MdSpi
    pMdApi->RegisterSpi(pMdSpi);        // 回调对象注入接口类
    pMdApi->RegisterFront(MDFRONT);     // 注册行情前置地址
    pMdApi->Init();                     //接口线程启动, 开始工作

  	//初始化traderApi
    pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
  	pTraderSpi = new CtpTraderSpi(pTraderApi);
  	pTraderApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderSpi);			// 注册事件类
  	pTraderApi->SubscribePublicTopic(THOST_TERT_RESTART);					// 注册公有流
  	pTraderApi->SubscribePrivateTopic(THOST_TERT_RESTART);			  // 注册私有流
  	pTraderApi->RegisterFront(TRADEFRONT);	// 注册交易前置地址
  	pTraderApi->Init();


  createThread *ct = new createThread();
  //cout<<"the address of ct is "<<ct<<endl;
  int start = ct->start();
  //cout<<"starting the thread"<<endl;
  if (!start){
    cout<<"the thread is not installed"<<endl;
    //创建线程没有成功，下次重新创建。
    start = ct->start();
  }
  //create the oracle thread

}

void application::Run() {
  int i;
  sleep(10);
  MdUserLogin(BrokerID, USERID, PASSWD);
  sleep(10);
  TraderUserLogin(BrokerID, USERID, PASSWD);
  sleep(10);
  GetAllInstrumentIds();
  sleep(10);
  SubscribeMarketData();
  while(1){

    time_t tt = time(NULL);
    tm* t= localtime(&tt);
    int hour = t->tm_hour;
    // cout<<hour<<endl;
    if (hour ==15 or hour == 2)
    {
    	sleep(10);
    	return;
    }
    sleep(120);
  }
  // while (1) {
  //   cin >> i;
  //   switch (i) {
  //     case 1: {
  //       //用户登录信息，包括md登录和trader登录
  //       // MD用户开始登录
  //       MdUserLogin(BrokerID, USERID, PASSWD);
		//     //trader 用户开始登陆
		//     //TraderUserLogin(BrokerID, USERID, PASSWD);
  //       break;
  //     }
  //     case 2: {
  //       // 查询所有的合约id信息，
  //       GetAllInstrumentIds();
  //       break;
  //     }
	 //  case 3: {
  //       // 订阅所有的合约信息
  //       SubscribeMarketData();
  //       break;
  //     }
  //     case 4: {
  //       // 程序退出，或者可以做别的操作。
  //       exit(0);
  //       break;
  //     }
  //   }
  // }
  pMdApi->Join();
  pTraderApi->Join();
}

void application::MdUserLogin(TThostFtdcBrokerIDType BrokerID,
                              TThostFtdcUserIDType userId,
                              TThostFtdcPasswordType passwd) {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, BrokerID);
  strcpy(req.UserID, userId);
  strcpy(req.Password, passwd);
  int ret = pMdApi->ReqUserLogin(&req, ++requestid_);
  cerr << " req md | send login..." << ((ret == 0) ? "success" : "fail") << endl;
}


void application::TraderUserLogin(TThostFtdcBrokerIDType BrokerID,
                              TThostFtdcUserIDType userId,
                              TThostFtdcPasswordType passwd) {
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BrokerID);
   strcpy(req.UserID, userId);
   strcpy(req.Password, passwd);
   int ret = pTraderApi->ReqUserLogin(&req, ++requestid_);
    cerr<<" request trader | send login in..."<<((ret == 0) ? "succeed" :"failed") << endl;

	//开始结算单请求并且确认

	// pTraderSpi->ReqQrySettlementInfo(BrokerID,userId,requestid_);
	// pTraderSpi->ReqSettlementInfoConfirm(BrokerID,userId,requestid_);
}

void application::GetAllInstrumentIds(){
	CThostFtdcQryInstrumentField req;
    memset(&req, 0, sizeof(req));   
	int ret = pTraderApi->ReqQryInstrument(&req, ++requestid_);
    cerr<<" request |send get all instruments..."<<((ret == 0) ? "succeed" :"failed") << endl;
}

void application::SubscribeMarketData() {
	set<string> instrumentIds = pTraderSpi->getSetInstruments();
	set<string>::iterator iter=instrumentIds.begin();  

  while(iter!=instrumentIds.end())  
  {  
      cout<<*iter<<endl;  
      char** tmp = new char* [1];
      tmp[0] = ChangeStrToChar(*iter);
      int ret = pMdApi->SubscribeMarketData(tmp, 1);
      // cerr << " req | send market md... "  <<*iter << ((ret == 0) ? " success" : "fail") << endl;

      ++iter;  
  }  

  // char** tmp = new char* [1];
  // tmp[0] = ChangeStrToChar("rb1801");
  // int ret = pMdApi->SubscribeMarketData(tmp, 1);

  // set<string> instruments_tmp;
  // instruments_tmp.insert("sn1801");
  // // instruments_tmp.insert("rb1801");
  // // instruments_tmp.insert("ru1801");
  // // instruments_tmp.insert("pb1711");
  // // instruments_tmp.insert("zn1711");
  // // instruments_tmp.insert("hc1801");
  // // instruments_tmp.insert("al1711");
  // // instruments_tmp.insert("cu1711");
  // // instruments_tmp.insert("ni1801");
  //  set<string>::iterator iter=instruments_tmp.begin();  

  //   // while(iter!=instrumentIds.end())  
  //  while(iter!=instruments_tmp.end())  
  //   {  
  //       // cout<<*iter<<endl;  
		// char** tmp = new char* [1];
		// tmp[0] = ChangeStrToChar(*iter);
		// int ret = pMdApi->SubscribeMarketData(tmp, 1);
  //       // cerr << " req | send market md... " << ((ret == 0) ? "success" : "fail") << endl;

  //       ++iter;  
  //   }  
}

application::~application()
{
    //dtor
}

