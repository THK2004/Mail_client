#pragma once
#include "Header.h"
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

int send_mail(string sender, vector<string> toReceiver, vector<string> ccReceiver, vector<string> bccReceiver, vector<string> filename);
string base64_encode(const string&);
string getTimeUTCplus7();
vector<string> getEncodedData(vector<string> filename);