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
std::string base64Decode(const std::string& base64String);
void savefiles(string filepath, string mailContent);
std::string generateUniqueFileNameWhenDownload(const std::string& filename, const std::string& filetype);