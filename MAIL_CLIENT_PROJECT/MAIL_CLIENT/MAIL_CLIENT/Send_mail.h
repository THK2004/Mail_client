#pragma once
#include "Header.h"
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <fstream>
#include <sstream>
#include <iomanip>

int send_mail();
string base64_encode(const string&);