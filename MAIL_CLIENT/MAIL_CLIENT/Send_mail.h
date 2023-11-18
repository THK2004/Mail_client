#pragma once
#include "Header.h"
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

int send_mail(
	string sender_name, 
	string sender_addr, 
	vector<string> toReceiver,
	vector<string> ccReceiver,
	vector<string> bccReceiver, 
	vector<string> filename
);
string base64_encode(const string&);
string getTimeUTCplus7();
vector<string> getEncodedData(vector<string> filename);