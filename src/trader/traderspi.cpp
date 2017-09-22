
#include "traderspi.h"



void CtpTraderSpi::OnFrontConnected()
{
	cerr<<" link the front.trader..succeed"<<endl;
}

void CtpTraderSpi::ReqUserLogin(TThostFtdcBrokerIDType	vAPPID,
	        TThostFtdcUserIDType	vUserId,	TThostFtdcPasswordType	vPasswd,int &requestId)
{

	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, vAPPID);
	strcpy(req.UserID, vUserId);
	strcpy(req.Password, vPasswd);
	int ret = pUserApi->ReqUserLogin(&req, ++requestId);
   cerr<<" request | send login in..."<<((ret == 0) ? "succeed" :"failed") << endl;
}

void CtpTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if ( !IsErrorRspInfo(pRspInfo) && pRspUserLogin ) {
    // 保存会话参数
		frontId = pRspUserLogin->FrontID;
		sessionId = pRspUserLogin->SessionID;
		int nextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		sprintf(orderRef, "%d", ++nextOrderRef);
    cerr<<" onreq | login in succeed ...current date:"<<pRspUserLogin->TradingDay<<endl;
  }
}

void CtpTraderSpi::ReqQrySettlementInfo(TThostFtdcBrokerIDType	vAPPID,
	        TThostFtdcUserIDType	vUserId,int &requestId)
{
	CThostFtdcQrySettlementInfoField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, vAPPID);
	strcpy(req.InvestorID, vUserId);
	int ret = pUserApi->ReqQrySettlementInfo(&req, ++requestId);
	cerr<<" request | send the confirm of requery..."<<((ret == 0)?"succeed" :"failed")<<endl;
}

void CtpTraderSpi::ReqSettlementInfoConfirm(TThostFtdcBrokerIDType	vAPPID,
	        TThostFtdcUserIDType	vUserId,int &requestId)
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, vAPPID);
	strcpy(req.InvestorID, vUserId);
	int ret = pUserApi->ReqSettlementInfoConfirm(&req, ++requestId);
	cerr<<" request | send confirm ..."<<((ret == 0)? "succeed" :"failed")<<endl;
}

void CtpTraderSpi::OnRspSettlementInfoConfirm(
        CThostFtdcSettlementInfoConfirmField  *pSettlementInfoConfirm,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cout<<"the function is called"<<endl;
	if( !IsErrorRspInfo(pRspInfo) && pSettlementInfoConfirm){
    cerr<<" on ret | confirm..."<<pSettlementInfoConfirm->InvestorID
      <<"...<"<<pSettlementInfoConfirm->ConfirmDate
      <<" "<<pSettlementInfoConfirm->ConfirmTime<<">...confirm"<<endl;
    }
    else{
       cout<<"the selletment info confirm is wrong"<<endl;
    }
}

void CtpTraderSpi::ReqQryInstrument(TThostFtdcInstrumentIDType instId,int &requestId)
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
   strcpy(req.InstrumentID, instId);//为空表示查询所有合约
	int ret = pUserApi->ReqQryInstrument(&req, ++requestId);
	cerr<<" request | send instrument request..."<<((ret == 0)?"succeed" :"failed")<<endl;
}

void CtpTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
         CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cout<<"this OnRspQryInstrument is called"<<endl;
	if ( !IsErrorRspInfo(pRspInfo) &&  pInstrument){
      cerr<<" on ret | instrumentid:"<<pInstrument->InstrumentID<<endl;

	  instrumentsIds.insert(pInstrument->InstrumentID);
	  //cout<<instrumentsIds.size()<<endl;
  }
}

set<string> CtpTraderSpi::getSetInstruments(){
	return instrumentsIds;
}



void CtpTraderSpi::OnFrontDisconnected(int nReason)
{
	cerr<<" on ret | link stop..."
	  << " reason=" << nReason << endl;
}


void CtpTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr<<"  on ret | heart break warming..."
	  << " TimerLapse = " << nTimeLapse << endl;
}

void CtpTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

bool CtpTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
  if (ret){
    basicPrint(pRspInfo->ErrorMsg);
    // cerr<<" ret error trader | "<<pRspInfo->ErrorMsg<<endl;
    cerr<<" ret error trader | "<<ConvertGb18030ToUtf8(pRspInfo->ErrorMsg)<<endl;
  }
	return ret;
}

char MapDirection(char src, bool toOrig=true){
  if(toOrig){
    if('b'==src||'B'==src){src='0';}else if('s'==src||'S'==src){src='1';}
  }else{
    if('0'==src){src='B';}else if('1'==src){src='S';}
  }
  return src;
}
char MapOffset(char src, bool toOrig=true){
  if(toOrig){
    if('o'==src||'O'==src){src='0';}
    else if('c'==src||'C'==src){src='1';}
    else if('j'==src||'J'==src){src='3';}
  }else{
    if('0'==src){src='O';}
    else if('1'==src){src='C';}
    else if('3'==src){src='J';}
  }
  return src;
}
