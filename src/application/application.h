#pragma once
#include "../md/mdspi.h"
#include "../trader/traderspi.h"
#include "../basicFun/basicFun.h"
#include "../createThread/createThread.h"
#include <unistd.h>
#include <iostream>
#include <ctime>

class application {
 public:
  application();
  void ConfigFun();
  void Init();
  void Run();
  void MdUserLogin(TThostFtdcBrokerIDType BrokerID, TThostFtdcUserIDType userId,
                   TThostFtdcPasswordType passwd);
  void TraderUserLogin(TThostFtdcBrokerIDType BrokerID, TThostFtdcUserIDType userId,
                   TThostFtdcPasswordType passwd);
  void SubscribeMarketData();
  void GetAllInstrumentIds();

  virtual ~application();

 protected:
 private:
  int requestid_;
	char* MDFRONT;
	char* TRADEFRONT;

	TThostFtdcBrokerIDType BrokerID;
	TThostFtdcUserIDType USERID;
	TThostFtdcPasswordType PASSWD;

	char** PPINSTRUMENTID;
	int NUMOFINSTRUMENTID;
  CtpMdSpi* pMdSpi;
  CtpTraderSpi* pTraderSpi;

  CThostFtdcMdApi* pMdApi;
  CThostFtdcTraderApi* pTraderApi;

   unsigned long updateID=0;
  
};

