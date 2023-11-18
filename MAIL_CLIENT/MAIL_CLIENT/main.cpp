#pragma comment(lib, "Ws2_32.lib")
#include "Send_mail.h"

using namespace std;

int main() {
    string sender_name = "Khanh Tran";
    string sender_addr = "sender@fit.hcmus.edu.vn";
    vector<string> toReceiver = { "receiver@yahoo.com.vn" };
    vector<string> ccReceiver = { "receiver2@gmail.com" };
    vector<string> bccReceiver = { "receiver3@whatisthis.org" };
    vector<string> filename = { "Test.txt", "Test.zip", "Test.docx"};

    send_mail(sender_name, sender_addr, toReceiver, ccReceiver, bccReceiver, filename);

    system("pause");
    return 0;
}