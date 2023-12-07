#pragma comment(lib, "Ws2_32.lib")
#include "Mail_client.h"
#include <thread>   //For a specific time we need to check mail server

using namespace std;

bool isContinue(bool a);

void checkMailServerEveryNs(
    const Config& configData,
    const string& user_addr,
    const int& pop3_server_port,
    const string& pop3_server_addr,
    const string& receiverMailAddr,
    const string& password
);

int main() {
    //Config
    string fileConfigName = "configFile.xml";
    Config configData;
    configData.readFileXml(fileConfigName);

    //Thread
    string test = "receiver@yahoo.com.vn";
    std::thread workerThread(
        checkMailServerEveryNs,
        cref(configData), 
        cref(configData.general.user_addr),
        cref(configData.general.pop3Server_port),
        cref(configData.general.server_addr),
        cref(test),
        cref(configData.general.password)
    );

    //Mail_client
    mail_client(configData);

    isContinue(0);

    workerThread.join();
    cout << "workerThread joined.\n";
    /*
    string user_name = "Khanh Tran";                        //Current login
    string user_addr = "sender@fit.hcmus.edu.vn";           //Current login
    int smtp_server_port = 2500;
    string smtp_server_addr = "127.0.0.1";
    vector<string> toReceiver = { "receiver@yahoo.com.vn" };
    vector<string> ccReceiver = { "receiver2@gmail.com" };
    vector<string> bccReceiver = { "receiver3@whatisthis.org" };
    vector<string> filename = { "Test.pdf", "Test.docx", "Test.txt", "Test.zip", "Test.jpg" };   //There is a 1MB file check
    string subject = "Test sending 5 item-attached mail (.pdf, .docx, .txt, .zip, .jpg) with code to try download";
    string content =
        "This is a test email.\r\n"
        "Hello\r\n"
        "How are you\r\n";
    */
    //Send_mail


    /*
    send_mail(
        user_name, 
        user_addr, 
        smtp_server_port, 
        smtp_server_addr,
        toReceiver, 
        ccReceiver, 
        bccReceiver, 
        filename, 
        subject, 
        content
    );
    */

    /*
    string user_addr = "sender@fit.hcmus.edu.vn";           //Current login
    int pop3_server_port = 1100;
    string pop3_server_addr = "127.0.0.1";
    string receiverMailAddr = "receiver@yahoo.com.vn";  //For USER command
    string password = "123456789";                      //For PASS command
    
    //Recv_mail
    recv_mail(
        configData,
        user_addr,
        pop3_server_port, 
        pop3_server_addr, 
        receiverMailAddr, 
        password
    );
    */


    std::cout << endl;
    system("pause");
    return 0;
}

void checkMailServerEveryNs(
    const Config& configData,
    const string& user_addr,
    const int& pop3_server_port,
    const string& pop3_server_addr,
    const string& receiverMailAddr,
    const string& password
) {
    while (isContinue(1)) {
        recv_mail(
            configData,
            user_addr,
            pop3_server_port,
            pop3_server_addr,
            receiverMailAddr,
            password
        );
        std::cout << "Updated\n";
        std::this_thread::sleep_for(std::chrono::seconds(configData.general.autoload));
    }
}

bool isContinue(bool a) {
    static bool isContinue = 1;
    if (a == 0) {
        isContinue = 0;
    }
    return isContinue;
}