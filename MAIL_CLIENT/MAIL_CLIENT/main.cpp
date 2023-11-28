#pragma comment(lib, "Ws2_32.lib")
#include "Send_mail.h"
#include "Recv_mail.h"

using namespace std;

int main() {
    //Send_mail
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

    //Recv_mail
    int pop3_server_port = 1100;
    string pop3_server_addr = "127.0.0.1";
    string receiverMailAddr = "receiver@yahoo.com.vn";  //For USER command
    string password = "123456789";                      //For PASS command
    int mailOrderToRecv = 8;
    
    recv_mail(
        user_addr,
        pop3_server_port, 
        pop3_server_addr, 
        receiverMailAddr, 
        password, 
        mailOrderToRecv
    );

    std::cout << endl;
    system("pause");
    return 0;
}