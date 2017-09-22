#include <iostream>
#include "application/application.h"
using namespace std;

pthread_mutex_t MUTEX;
queue<CThostFtdcDepthMarketDataField> md_queue;

int main(){
  application* app = new application();
  app->ConfigFun();
  app->Init();
  app->Run();

  return 0;
}