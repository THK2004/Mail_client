#pragma once
#include "Header.h"
#include "Config.h"
#include <sstream>
#include <fstream>
#include <chrono>
#include <WinSock2.h>
#include <Ws2tcpip.h>

class Mail {
public:
    bool isRead = 0;
    char mailPath[150];
    char sender[99];
    char subject[250];
    Mail();
    Mail(bool, const char*, const char*, const char*);
};

void recv_mail(
    Config configData,
    string user_addr,
    int pop3_server_port,
    string pop3_server_addr,
    string userMailAddr,
    string password
);

void createUserFolderAndItsSubFolder(string user_addr);
std::string base64Decode(const std::string& base64String);
void saveattachments(string mailContent);
std::string generateUniqueFileNameWhenDownload(const std::string& filename, const std::string& filetype);
void writeToManagement(string filepath, string mailName, string mailContent);
string filtingMailContent(Config configData, string mailContent);