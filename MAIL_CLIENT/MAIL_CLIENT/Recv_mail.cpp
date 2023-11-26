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
            string mailOrder = clientRequests[i].substr(clientRequests[i].find(' ') + 1, clientRequests[i].size() - 1);
            unsigned long long mailBytes = mailOrderAndItsByte[std::stoi(mailOrder) - 1].second;
            char* buffer = new char[mailBytes + 1000];
            for (int i = (int)mailBytes; i < mailBytes + 1000; i++) {
                buffer[i] = '\0';
            }
            if (recv(clientSocket, buffer, (int)mailBytes + 1000, 0) == SOCKET_ERROR) {
                std::cerr << "Failed to receive server message." << std::endl;
                closesocket(clientSocket);
                WSACleanup();
                system("pause");
                exit(1);
            }

            std::cout << "SERVER: " << buffer;

            string path = "Mailbox/";
            string filter = "Inbox/";

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

            //remove '\r' from buffer
            tmp = strlen(buffer);
            size_t j = 0;
            for (size_t i = 0; i < tmp; i++) {
                if (buffer[i] != '\r')
                    buffer[j++] = buffer[i];
            }
            buffer[j] = '\0';

            //remove +OK line from buffer
            tmp = strlen(buffer);
            j = 0;
            for (j = 0; j < tmp; j++) {
                if (buffer[j] == '\n') {
                    break;
                }
            }
            std::memmove(buffer, buffer + j + 1, tmp - j);

            //remove . from last line
            tmp = strlen(buffer);
            buffer[tmp - 2] = '\0';

            //Create a user folder if not created;
            createUserFolderAndItsSubFolder(user_addr);

            //Write down buffer to file
            ofstream ofs(path + user_addr + "/" + filter + filename + ".msg");
            if (!ofs.is_open()) {
                std::cerr << "Failed to open .msg file to write mail content down\n";
            }

            ofs << buffer;

            ofs.close();
            delete[] buffer;
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