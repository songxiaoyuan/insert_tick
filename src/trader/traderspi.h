#ifndef ORDER_TRADERSPI_H_
#define ORDER_TRADERSPI_H_
#pragma once

#include "../api/ThostFtdcTraderApi.h"
#include "../basicFun/basicFun.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <set>

using namespace std;


class CtpTraderSpi : public CThostFtdcTraderSpi
{
public:
   CtpTraderSpi(CThostFtdcTraderApi* api):pUserApi(api){};

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);



	////错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	virtual void OnHeartBeatWarning(int nTimeLapse);


public:
	///用户登录请求
	void ReqUserLogin(TThostFtdcBrokerIDType	appId,
	        TThostFtdcUserIDType	userId,	TThostFtdcPasswordType	passwd,int &requestId);
    ///请求查询投资者结算结果
	void ReqQrySettlementInfo(TThostFtdcBrokerIDType	vAPPID,
	        TThostFtdcUserIDType	vUserId,int &requestId);
	///投资者结算结果确认
	void ReqSettlementInfoConfirm(TThostFtdcBrokerIDType	vAPPID,
	        TThostFtdcUserIDType	vUserId,int &requestId);
	///请求查询合约
	void ReqQryInstrument(TThostFtdcInstrumentIDType instId,int &requestId);

	/// 是否收到成功的响应
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

	set<string> getSetInstruments();

private:
  CThostFtdcTraderApi* pUserApi;
  // 会话参数
  int	 frontId;	
  int	 sessionId;	
  char orderRef[13];

  set<string> instrumentsIds;

  vector<CThostFtdcOrderField*> orderList;
  vector<CThostFtdcTradeField*> tradeList;

};

#endif
