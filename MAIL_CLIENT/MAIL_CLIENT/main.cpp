#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <string>
#include <cstring>
#include <Ws2tcpip.h>

using namespace std;

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Connect to the SMTP server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(2500);  // SMTP port
    InetPton(AF_INET, TEXT("127.0.0.1"), &serverAddress.sin_addr.s_addr);  // Local server address

    if (connect(clientSocket, (sockaddr*)(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to the server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Receive the server's welcome message
    char serverMessage[1024];
    memset(serverMessage, '\0', sizeof(serverMessage));

    if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to receive server message." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server: " << serverMessage;
    memset(serverMessage, '\0', sizeof(serverMessage));

    // Send the EHLO command
    std::string ehloCommand = "EHLO localhost\r\n";
    if (send(clientSocket, ehloCommand.c_str(), static_cast<int>(ehloCommand.length()), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send EHLO command." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << ehloCommand;


    // Receive the response to the EHLO command
    if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to receive server response." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server: " << serverMessage;
    memset(serverMessage, '\0', sizeof(serverMessage));

    // Send the MAIL FROM command
    std::string mailFromCommand = "MAIL FROM: <sender@fit.hcmus.edu.vn>\r\n";
    if (send(clientSocket, mailFromCommand.c_str(), static_cast<int>(mailFromCommand.length()), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send MAIL FROM command." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << mailFromCommand;

    // Receive the response to the MAIL FROM command
    if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to receive server response." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server: " << serverMessage;
    memset(serverMessage, '\0', sizeof(serverMessage));

    // Send the RCPT TO command
    std::string rcptToCommand = "RCPT TO: <receiver@yahoo.com.vn>\r\n";
    if (send(clientSocket, rcptToCommand.c_str(), static_cast<int>(rcptToCommand.length()), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send RCPT TO command." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << rcptToCommand;


    // Receive the response to the RCPT TO command
    if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to receive server response." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server: " << serverMessage;
    memset(serverMessage, '\0', sizeof(serverMessage));

    // Send the DATA command
    std::string dataCommand = "DATA\r\n";
    if (send(clientSocket, dataCommand.c_str(), static_cast<int>(dataCommand.length()), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send DATA command." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << dataCommand;

    // Receive the response to the DATA command
    if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to receive server response." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server: " << serverMessage;
    memset(serverMessage, '\0', sizeof(serverMessage));

    // Send the email content
    std::string emailContent = "Subject: Test Email 2\r\n\r\nThis is a test email.\r\n.\r\n";
    if (send(clientSocket, emailContent.c_str(), static_cast<int>(emailContent.length()), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send email content." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << emailContent;

    // Receive the response to the email content
    if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to receive server response." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server: " << serverMessage;
    memset(serverMessage, '\0', sizeof(serverMessage));

    // Send the QUIT command
    std::string quitCommand = "QUIT\r\n";
    if (send(clientSocket, quitCommand.c_str(), static_cast<int>(quitCommand.length()), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send QUIT command." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << quitCommand;

    // Receive the response to the QUIT command
    if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to receive server response." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server: " << serverMessage;

    // Close the socket and cleanup
    closesocket(clientSocket);
    WSACleanup();


    return 0;
}