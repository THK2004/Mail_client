#pragma once
#include "Header.h"
#include <sstream>
#include <fstream>
#include <chrono>
#include <WinSock2.h>
#include <Ws2tcpip.h>

void recv_mail(
    string user_addr,
    int pop3_server_port,
    string pop3_server_addr,
    string userMailAddr,
    string password,
    int mailOrderToRecv
);

void createUserFolderAndItsSubFolder(string user_addr);