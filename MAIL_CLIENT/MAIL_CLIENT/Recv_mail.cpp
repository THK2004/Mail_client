#include "Recv_mail.h"

void recv_mail(
    string user_addr,
    int pop3_server_port,
    string pop3_server_addr,
    string userMailAddr,
    string password,
    int mailOrderToRecv
) {
    std::wstring stemp = std::wstring(pop3_server_addr.begin(), pop3_server_addr.end());
    LPCWSTR server_addr = stemp.c_str();

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        exit(1);
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        exit(1);
    }

    // Connect to the SMTP server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(pop3_server_port);  // POP3 port
    InetPton(AF_INET, server_addr, &serverAddress.sin_addr.s_addr);  // POP3 server address

    if (connect(clientSocket, (sockaddr*)(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to the server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        system("pause");
        exit(1);
    }

    // Receive the server greeting
    char serverMessage[BUFFER_SIZE];
    memset(serverMessage, '\0', sizeof(serverMessage));
    // Receive the server's welcome message
    if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to receive server message." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        system("pause");
        exit(1);
    }
    std::cout << "SERVER: " << serverMessage;
    memset(serverMessage, '\0', sizeof(serverMessage));

    //USER Command
    std::string userCommand = "USER " + userMailAddr + "\r\n";

    //PASSWORD Command
    std::string passCommand = "PASS " + password + "\r\n";

    //RETR Command
    std::string retrCommand = "RETR " + std::to_string(mailOrderToRecv) + "\r\n";

    vector<string> clientRequests = {
        "CAPA\r\n",
        userCommand,
        passCommand,
        "STAT\r\n",
        "LIST\r\n",
        "UIDL\r\n",
        retrCommand,
        "QUIT\r\n"
    };

    //Communicate with server
    int numOfMessage = (int)clientRequests.size();
    vector<pair<string, unsigned long long>> mailOrderAndItsByte;
    vector<pair<string, string>> mailOrderAndItsName;
    for (int i = 0; i < numOfMessage; i++) {
        // Send message
        if (send(clientSocket, clientRequests[i].c_str(), static_cast<int>(clientRequests[i].length()), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send message." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            system("pause");
            exit(1);
        }
        std::cout << "CLIENT: " << clientRequests[i];

        // Receive server's response
        if (clientRequests[i].substr(0, 4) == "LIST") {
            if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
                std::cerr << "Failed to receive server message." << std::endl;
                closesocket(clientSocket);
                WSACleanup();
                system("pause");
                exit(1);
            }

            std::cout << "SERVER: " << serverMessage;

            stringstream ss(serverMessage);
            string token;
            while (!ss.eof()) {
                getline(ss, token, '\n');
                int n = (int)token.size();
                if (token[0] != '+' && token[0] != '.' && n > 3) {
                    string mailOrder = token.substr(0, token.find(' '));
                    unsigned long long itsBtye = std::stoi(token.substr(token.find(' ') + 1, n - 1));
                    mailOrderAndItsByte.push_back(std::make_pair(mailOrder, itsBtye));
                }
            }
            //for (int i = 0; i < mailOrderAndItsByte.size(); i++) {
            //    cout << mailOrderAndItsByte[i].first << " " << mailOrderAndItsByte[i].second << endl;
            //}
            memset(serverMessage, '\0', sizeof(serverMessage));
        }
        else if (clientRequests[i].substr(0, 4) == "UIDL") {
            if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
                std::cerr << "Failed to receive server message." << std::endl;
                closesocket(clientSocket);
                WSACleanup();
                system("pause");
                exit(1);
            }

            std::cout << "SERVER: " << serverMessage;

            stringstream ss(serverMessage);
            string token;
            while (!ss.eof()) {
                getline(ss, token, '\n');
                int n = (int)token.size();
                if (token[0] != '+' && token[0] != '.' && n > 3) {
                    string mailOrder = token.substr(0, token.find(' '));
                    string itsName = token.substr(token.find(' ') + 1, n - 1);
                    mailOrderAndItsName.push_back(std::make_pair(mailOrder, itsName));
                }
            }
            //for (int i = 0; i < mailOrderAndItsName.size(); i++) {
            //    cout << mailOrderAndItsName[i].first << " " << mailOrderAndItsName[i].second << endl;
            //}
            memset(serverMessage, '\0', sizeof(serverMessage));
        }
        else if (clientRequests[i].substr(0, 4) == "RETR") {
            //string mailOrder = clientRequests[i].substr(clientRequests[i].find(' ') + 1, clientRequests[i].size() - 1);
            char buffer[BUFFER_SIZE];
            int bytesRead = 0;
            string mailContent;
            string previousBuffer;
            do {
                bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
                if (bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    mailContent += buffer;
                    string combinedbuffer = previousBuffer + buffer;
                    if (combinedbuffer.find("\r\n.\r\n") != std::string::npos ) {
                         break;
                    }
                    previousBuffer = buffer;
                }
                else if (bytesRead == 0) {
                    break;
                }
                else {
                    std::cerr << "Failed to receive data from server." << std::endl;
                    closesocket(clientSocket);
                    WSACleanup();
                    system("pause");
                    exit(1);
                }
            } while (bytesRead > 0);

            std::cout << "SERVER: " << mailContent;

            //Get time to make file name
            auto now = std::chrono::system_clock::now();
            std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
            currentTime += 7 * 3600;
            std::time(&currentTime);
            char timeString[26];
            ctime_s(timeString, sizeof(timeString), &currentTime);
            timeString[24] = '\0';          //replace \n here
            size_t tmp = strlen(timeString);
            for (int i = 0; i < tmp; i++) {
                if (timeString[i] == ' ' || timeString[i] == ':')
                    timeString[i] = '_';
            }
            string filename(timeString);

            //remove '\r' from mailContent
            tmp = mailContent.find('\r');
            while (tmp != std::string::npos) {
                mailContent.erase(tmp, 1);
                tmp = mailContent.find('\r', tmp);
            }
            
            //remove +OK line from buffer
            tmp = mailContent.find('\n');
            if (tmp != std::string::npos) {
                mailContent.erase(0, tmp + 1);
            }

            //remove . from last line
            if (mailContent.size() >= 2) {
                mailContent.erase(mailContent.size() - 2);
            }

            //Create a user folder if not created;
            createUserFolderAndItsSubFolder(user_addr);

            //Create file path
            string path = "Mailbox/";
            string filter = "Inbox/";
            string filepath = path + user_addr + "/" + filter;

            //Write down buffer to .msg file
            ofstream ofs(filepath + filename + ".msg");
            if (!ofs.is_open()) {
                std::cerr << "Failed to open .msg file to write mail content down\n";
            }

            ofs << mailContent;

            ofs.close();
            
            //Save file from mail
            savefiles(filepath, mailContent);
        }
        else {
            if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
                std::cerr << "Failed to receive server message." << std::endl;
                closesocket(clientSocket);
                WSACleanup();
                system("pause");
                exit(1);
            }

            std::cout << "SERVER: " << serverMessage;
            memset(serverMessage, '\0', sizeof(serverMessage));
        }
    }
}

void createUserFolderAndItsSubFolder(string user_addr) {
    std::wstring folderpath = L"Mailbox\\";
    for (const char c : user_addr) {
        folderpath += (wchar_t)c;
    }

    if (CreateDirectoryW(folderpath.c_str(), NULL)) {
        std::wcout << L"User folder created successfully." << std::endl;
    }
    else {
        DWORD errorCode = GetLastError();

        if (errorCode == ERROR_ALREADY_EXISTS) {
            std::wcout << L"User already exists." << std::endl;
        }
        else if (errorCode == ERROR_ACCESS_DENIED) {
            std::wcout << L"Access denied. Insufficient permissions." << std::endl;
        }
        else {
            std::wcout << L"Failed to create user folder. Error code: " << errorCode << std::endl;
        }
    }

    wstring subfolder[] = {
        L"Important",
        L"Inbox",
        L"Project",
        L"Spam",
        L"Work"
    };

    for (int i = 0; i < 5; ++i) {
        std::wstring subfolderpath = folderpath + L"\\" + subfolder[i];

        if (CreateDirectoryW(subfolderpath.c_str(), NULL)) {
            std::wcout << subfolder[i] << L" created successfully." << std::endl;
        }
        else {
            DWORD errorCode = GetLastError();
            std::wcout << L"Failed to create folder " << subfolder[i] << L". Error code: " << errorCode << std::endl;
        }
    }
}

std::string base64Decode(const std::string& base64String) {
    const std::string base64Chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string decodedString;
    int val = 0, valb = -8;
    for (char c : base64String)
    {
        if (c == '=')
            break;
        if (c >= 'A' && c <= 'Z')
            c -= 'A';
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 26;
        else if (c >= '0' && c <= '9')
            c -= '0' - 52;
        else if (c == '+')
            c = 62;
        else if (c == '/')
            c = 63;
        else
            continue;

        val = (val << 6) + c;
        valb += 6;
        if (valb >= 0)
        {
            decodedString.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return decodedString;
}

void savefiles(string filepath, string mailContent) {
    vector<pair<string, size_t>> listOfAttachedFileNameAndTheirPos;         //list of filename include what after '.' (ex: .pdf)

    size_t pos = mailContent.find("Content-Disposition: attachment");
    while (pos != std::string::npos) {
        string filename;
        size_t start_filename = mailContent.find('=', pos + 1);
        size_t end_filename = mailContent.find('\n', pos + 1);
        for (size_t i = start_filename + 2; i < end_filename - 1; i++) {
            filename += mailContent[i];
        }
        size_t startDataIndex = mailContent.find('\n', mailContent.find("Content-Transfer-Encoding: base64", pos + 1));
        listOfAttachedFileNameAndTheirPos.push_back(std::make_pair(filename, startDataIndex + 2));

        pos = mailContent.find("Content-Disposition: attachment", pos + 1);
    }

    size_t numOfAttachedFile = listOfAttachedFileNameAndTheirPos.size();

    std::cout << "\nThere is/are " << numOfAttachedFile << " attached item(s) in the mail:\n";
    for (int i = 0; i < numOfAttachedFile; i++) {
        std::cout << i + 1 << ". " << listOfAttachedFileNameAndTheirPos[i].first << endl;
    }
    
    bool isLooping = 1;
    while (isLooping) {
        cout << "Which one do you want to save?\n("
                "If more than 1, use '+' to demmand. Ex: 2+5+1 to download\n"
                "file 2nd, 5th, 1st in order. If none, enter 0): ";
        string command;
        getline(cin, command);
        size_t size = command.size();

        if (size == 0) {
            cout << "No command received.\n";
            isLooping = 1;
        }
        else if (size == 1) {
            if (isalpha(command[0])) {
                cout << "Wrong input.\n";
                isLooping = 1;
            }
            else if (isdigit(command[0])) {
                if (command[0] - 48 == 0) {
                    isLooping = 0;
                }
                else if (command[0] - 48 > numOfAttachedFile) {
                    cout << "There is no such file.\n";
                    isLooping = 1;
                }
                else {
                    //Get file at command
                    string filefullname = listOfAttachedFileNameAndTheirPos[command[0] - 48 - 1].first;
                    string filetype = filefullname.substr(filefullname.find('.'));              //.pdf or something
                    string filename = filefullname.substr(0, filefullname.find('.'));
                    string uniquefilename = generateUniqueFileNameWhenDownload(filename, filetype);
                    size_t start = listOfAttachedFileNameAndTheirPos[command[0] - 48 - 1].second;
                    size_t end = mailContent.find("--", start);
                    string base64Data;
                    if (command[0] - 48 != numOfAttachedFile) {
                        base64Data = mailContent.substr(start, end - start);
                    }
                    else {
                        base64Data = mailContent.substr(start, end - start - 1);
                    }
                    isLooping = 0;
                    string decodedBase64Data = base64Decode(base64Data);

                    ofstream ofs("Download/" + uniquefilename + filetype, std::ios::binary);

                    if (!ofs) {
                        cout << "Cannot open download file path";
                        return;
                    }

                    ofs << decodedBase64Data;

                    ofs.close();

                    isLooping = 0;
                }
            }
        }
        else {
            bool isWrong = 0;
            vector<int> attachmentToBeDownLoad;
            for (const char A : command) {
                if (!isdigit(A) && A != '+') {
                    cout << "Wrong input.\n";
                    isWrong = 1;
                    isLooping = 1;
                    break;
                }
            }
            if (!isWrong) {
                size_t plusPos = command.find('+');
                if (plusPos == 0 || command[size - 1] == '+') {
                    cout << "Wrong input.\n";
                    isWrong = 1;
                    isLooping = 1;
                }
                if (!isWrong) {
                    if (plusPos == std::string::npos) {
                        if (std::stoi(command) > numOfAttachedFile) {
                            cout << "There is a non-existing option in your command.\n";
                            isLooping = 1;
                            isWrong = 1;
                        }
                        else {
                            //Get file at command
                            int command_ = std::stoi(command);
                            string filefullname = listOfAttachedFileNameAndTheirPos[command_ - 1].first;
                            string filetype = filefullname.substr(filefullname.find('.'));              //.pdf or something
                            string filename = filefullname.substr(0, filefullname.find('.'));
                            string uniquefilename = generateUniqueFileNameWhenDownload(filename, filetype);
                            size_t start = listOfAttachedFileNameAndTheirPos[command_ - 1].second;
                            size_t end = mailContent.find("--", start);
                            string base64Data;
                            if (command[0] - 48 != numOfAttachedFile) {
                                base64Data = mailContent.substr(start, end - start);
                            }
                            else {
                                base64Data = mailContent.substr(start, end - start - 1);
                            }
                            isLooping = 0;
                            string decodedBase64Data = base64Decode(base64Data);

                            ofstream ofs("Download/" + uniquefilename + filetype, std::ios::binary);

                            if (!ofs) {
                                cout << "Cannot open download file path";
                                return;
                            }

                            ofs << decodedBase64Data;

                            ofs.close();

                            isLooping = 0;
                        }
                    }
                }
                size_t previousPlusPos = 0;
                while (!isWrong && plusPos != std::string::npos) {
                    if (command[plusPos + 1] == '+') {
                        cout << "Wrong input.\n";
                        isWrong = 1;
                        isLooping = 1;
                    }
                    if (!isWrong) {
                        string toDownLoad = command.substr(previousPlusPos, plusPos - previousPlusPos);
                        attachmentToBeDownLoad.push_back(std::stoi(toDownLoad));
                    }

                    previousPlusPos = plusPos + 1;
                    plusPos = command.find('+', plusPos + 1);
                }
                if (!isWrong) {
                    attachmentToBeDownLoad.push_back(std::stoi(command.substr(previousPlusPos)));
                }
            }
            int numOfDownLoad = attachmentToBeDownLoad.size();
            if (!isWrong) {
                for (int i = 0; i < numOfDownLoad; i++) {
                    if (attachmentToBeDownLoad[i] > numOfAttachedFile) {
                        cout << "There is a non-existing option in your command.\n";
                        isLooping = 1;
                        isWrong = 1;
                        break;
                    }
                }
            }
            if (!isWrong) {
                for (int i = 0; i < numOfDownLoad; i++) {
                    //get files
                    string filefullname = listOfAttachedFileNameAndTheirPos[attachmentToBeDownLoad[i] - 1].first;
                    string filetype = filefullname.substr(filefullname.find('.'));              //.pdf or something
                    string filename = filefullname.substr(0, filefullname.find('.'));
                    string uniquefilename = generateUniqueFileNameWhenDownload(filename, filetype);
                    size_t start = listOfAttachedFileNameAndTheirPos[attachmentToBeDownLoad[i] - 1].second;
                    size_t end = mailContent.find("--", start);
                    string base64Data;
                    if (command[0] - 48 != numOfAttachedFile) {
                        base64Data = mailContent.substr(start, end - start);
                    }
                    else {
                        base64Data = mailContent.substr(start, end - start - 1);
                    }
                    isLooping = 0;
                    string decodedBase64Data = base64Decode(base64Data);

                    ofstream ofs("Download/" + uniquefilename + filetype, std::ios::binary);

                    if (!ofs) {
                        cout << "Cannot open download file path";
                        return;
                    }

                    ofs << decodedBase64Data;

                    ofs.close();
                }

                isLooping = 0;
            }
        }
    }
}

std::string generateUniqueFileNameWhenDownload(const std::string& filename, const std::string& filetype) {
    std::string uniqueName = filename;
    int counter = 1;

    while (std::ifstream("Download/" + uniqueName + filetype).good()) {
        uniqueName = filename + " (" + std::to_string(counter) + ")";
        counter++;
    }

    return uniqueName;
}