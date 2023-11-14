#include "Send_mail.h"

int send_mail() {
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

    char serverMessage[1024];
    memset(serverMessage, '\0', sizeof(serverMessage));
    // Receive the server's welcome message
    if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to receive server message." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Server: " << serverMessage;
    memset(serverMessage, '\0', sizeof(serverMessage));

    ifstream ifs("FileToSend/Test.txt", std::ios::binary);
    if (!ifs) {
        std::cerr << "Fail to open file";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    string attachmentContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    string encodedAttachment = base64_encode(attachmentContent);

    string boundary = "dsakdmasdadkada";
    string mailContent =
        "Content_Type: multipart/mixed; boundary=\"" + boundary + "\"\r\n"
        "To: receiver@yahoo.com.vn\r\n"
        "Cc: receiver2@gmail.com\r\n"
        "From: Khanh Tran <sender@fit.hcmus.edu.vn>\r\n"
        "Subject: Test sending attached mail with code\r\n\r\n"
        "This is a multi-part message in MIME format.\r\n"
        "--" + boundary + "\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "This is a test email.\r\nHello\r\nHow are you\r\n\r\n"
        "--" + boundary + "\r\n"
        "Content-Type: text/plain; charset=UTF-8; name=\"Test.txt\"\r\n"
        "Content-Disposition: attachment; filename=\"Test.txt\"\r\n"
        "Content-Transfer-Encoding: base64\r\n\r\n"
        + encodedAttachment + "\r\n\r\n"
        "--" + boundary + "--\r\n"
        ".\r\n";

    vector<string> mail = {
        "EHLO localhost\r\n",
        "MAIL FROM: <sender@fit.hcmus.edu.vn>\r\n",
        "RCPT TO: <receiver@yahoo.com.vn>\r\n",
        "RCPT TO: <receiver2@gmail.com>\r\n",
        "RCPT TO: <receiver3@whatisthis.org>\r\n",
        "DATA\r\n",
        mailContent,
        "QUIT\r\n"
    };

    int numOfMessage = (int)mail.size();

    for (int i = 0; i < numOfMessage; i++) {
        // Send message
        if (send(clientSocket, mail[i].c_str(), static_cast<int>(mail[i].length()), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send message." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        std::cout << "Client: " << mail[i];

        // Receive server's response
        if (recv(clientSocket, serverMessage, sizeof(serverMessage), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to receive server message." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Server: " << serverMessage;
        memset(serverMessage, '\0', sizeof(serverMessage));
    }

    // Close the socket and cleanup
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

string base64_encode(const string& input) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::stringstream encoded_stream;
    size_t input_length = input.size();

    for (size_t i = 0; i < input_length; i += 3) {
        unsigned char char1 = input[i];
        unsigned char char2 = (i + 1 < input_length) ? input[i + 1] : 0;
        unsigned char char3 = (i + 2 < input_length) ? input[i + 2] : 0;

        unsigned char enc1 = char1 >> 2;
        unsigned char enc2 = ((char1 & 0x3) << 4) | (char2 >> 4);
        unsigned char enc3 = ((char2 & 0xF) << 2) | (char3 >> 6);
        unsigned char enc4 = char3 & 0x3F;

        encoded_stream << base64_chars[enc1] << base64_chars[enc2]
            << ((i + 1 < input_length) ? base64_chars[enc3] : '=')
            << ((i + 2 < input_length) ? base64_chars[enc4] : '=');
    }

    return encoded_stream.str();
}