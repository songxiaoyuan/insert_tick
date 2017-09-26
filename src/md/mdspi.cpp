#include "mdspi.h"
using namespace std;

extern pthread_mutex_t MUTEX;
extern queue<CThostFtdcDepthMarketDataField> md_queue;

void CtpMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast) {
  IsErrorRspInfo(pRspInfo);
}

void CtpMdSpi::OnFrontDisconnected(int nReason) {
  cerr << " return | break..."
       << " reason=" << nReason << endl;
}

void CtpMdSpi::OnHeartBeatWarning(int nTimeLapse) {
  cerr << " return | heart warming..."
       << " TimerLapse = " << nTimeLapse << endl;
}

void CtpMdSpi::OnFrontConnected() {
  cerr << " connect md ...success" << endl;
}


void CtpMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) {
  if (!IsErrorRspInfo(pRspInfo) && pRspUserLogin) {
    cerr << " on ret | login in succeed...current date:" << pRspUserLogin->TradingDay
         << endl;
  }
}

void CtpMdSpi::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  // cerr << " on req |  submit market ...succeed" << endl;
}

void CtpMdSpi::OnRspUnSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  cerr << " on req |  remove the market date...succeed" << endl;
}

void CtpMdSpi::OnRtnDepthMarketData(
    CThostFtdcDepthMarketDataField *pDepthMarketData) {

	CThostFtdcDepthMarketDataField temp;
	memset(&temp,0,sizeof(CThostFtdcDepthMarketDataField));
	memcpy(&temp,pDepthMarketData,sizeof(CThostFtdcDepthMarketDataField));

	time_t tt = time(NULL);
	tm* t= localtime(&tt);

	char tmp[9]={0};
	sprintf(tmp,"%02d:%02d:%02d",t->tm_hour,t->tm_min,t->tm_sec);
	strcpy(temp.ExchangeID,tmp);

	pthread_mutex_lock(&MUTEX);
	md_queue.push(temp);
	pthread_mutex_unlock(&MUTEX);
  //string InstrumentID = pDepthMarketData->InstrumentID;
  //cout<<InstrumentID<<":"<<pDepthMarketData->LastPrice<<endl;
 // strToFile(InstrumentID);

/*
  cout << pDepthMarketData->InstrumentID << endl;
  cout << pDepthMarketData->TradingDay << endl;
  cout << pDepthMarketData->LastPrice << endl;
  cout << pDepthMarketData->Volume << endl;
  cout << pDepthMarketData->BidPrice1 << endl;
  cout << pDepthMarketData->AskPrice1 << endl;
  */
}

bool CtpMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
  bool ret = ((pRspInfo) && (pRspInfo->ErrorID != 0));
  if (ret){
    cerr<<" on req md error| "<<pRspInfo->ErrorMsg<<endl;
  }
  return ret;
}
