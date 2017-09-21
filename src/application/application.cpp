#include "./application.h"

extern pthread_mutex_t MUTEX;
extern queue<CThostFtdcDepthMarketDataField> md_queue;


application::application() {
  // ctor
	requestid_ = 1;
}

void application::ConfigFun() {
    //此函数主要是用来获取一些配置信息包括前置地址等。通过读取config.txt来获得信息。
	string path = "config.txt";
	unordered_map<string,string> ret =  GetConfigInfo(path);
	MDFRONT=ChangeStrToChar(ret["MDFRONT"]);
	TRADEFRONT=ChangeStrToChar(ret["TRADERFRONT"]);
	strcpy(APPID,ChangeStrToChar(ret["APPID"]));
	strcpy(USERID,ChangeStrToChar(ret["USERID"]));
	strcpy(PASSWD,ChangeStrToChar(ret["PASSWD"]));
	
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

}

void application::Run() {
  int i;
  cerr << "-----------------------------------------------" << endl;
  cerr << "[1] userLogin     "<<endl;
  cerr << "[2] userSubscribe "<< endl;
  cerr << "[3] Exit    " <<endl;
  cerr << "----------------------------------------------" << endl;
  while (1) {
    cin >> i;
    switch (i) {
      case 1: {
        //用户登录信息，包括md登录和trader登录
        // MD用户开始登录
        MdUserLogin(APPID, USERID, PASSWD);
		//trader 用户开始登陆
		TraderUserLogin(APPID, USERID, PASSWD);
        break;
      }
      case 2: {
        // 查询所有的合约id信息，
        GetAllInstrumentIds();
        break;
      }
	  case 3: {
        // 订阅所有的合约信息
        SubscribeMarketData();
        break;
      }
      case 4: {
        // 程序退出，或者可以做别的操作。
        exit(0);
        break;
      }
    }
  }
  pMdApi->Join();
  pTraderApi->Join();
}

void application::MdUserLogin(TThostFtdcBrokerIDType appId,
                              TThostFtdcUserIDType userId,
                              TThostFtdcPasswordType passwd) {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, APPID);
  strcpy(req.UserID, userId);
  strcpy(req.Password, passwd);
  int ret = pMdApi->ReqUserLogin(&req, ++requestid_);
  cerr << " req | send login..." << ((ret == 0) ? "success" : "fail") << endl;
}


void application::TraderUserLogin(TThostFtdcBrokerIDType appId,
                              TThostFtdcUserIDType userId,
                              TThostFtdcPasswordType passwd) {
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, APPID);
   strcpy(req.UserID, userId);
   strcpy(req.Password, passwd);
   int ret = pTraderApi->ReqUserLogin(&req, ++requestid_);
    cerr<<" request | send login in..."<<((ret == 0) ? "succeed" :"failed") << endl;

	//开始结算单请求并且确认

	pTraderSpi->ReqQrySettlementInfo(appId,userId,requestid_);
	pTraderSpi->ReqSettlementInfoConfirm(appId,userId,requestid_);
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
        cerr << " req | send market md... " << ((ret == 0) ? "success" : "fail") << endl;

        ++iter;  
    }  
}

application::~application()
{
    //dtor
}

