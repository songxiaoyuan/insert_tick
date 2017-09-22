#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iconv.h>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;

unordered_map<string,string> GetConfigInfo(string path);
vector<string> split(string target,string pattern);
char * ChangeStrToChar(string str);
void strToFile(string error);
void convert(char *inbuf,int inlen,char *outbuf,int outlen);
string ConvertGb18030ToUtf8(char message[]);
void basicPrint(string error);

