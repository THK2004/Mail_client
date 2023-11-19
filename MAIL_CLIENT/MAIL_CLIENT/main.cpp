#pragma comment(lib, "Ws2_32.lib")
#include "Send_mail.h"

using namespace std;

int main() {
    string sender_name = "Khanh Tran";
    string sender_addr = "sender@fit.hcmus.edu.vn";
    vector<string> toReceiver = { "receiver@yahoo.com.vn" };
    vector<string> ccReceiver = { "receiver2@gmail.com" };
    vector<string> bccReceiver = { "receiver3@whatisthis.org" };
    vector<string> filename = { "Test.txt", "Test.zip", "Test.docx"};   //There is a 5MB file check
    string subject = "Test sending 3 item-attached mail with code";
    string content =
        "This is a test email.\r\n"
        "Hello\r\n"
        "How are you\r\n";

    send_mail(sender_name, sender_addr, toReceiver, ccReceiver, bccReceiver, filename, subject, content);

    system("pause");
    return 0;
}