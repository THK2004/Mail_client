#pragma comment(lib, "Ws2_32.lib")
#include "Send_mail.h"

using namespace std;

int main() {
    string sender = "sender@fit.hcmus.edu.vn";
    vector<string> toReceiver = { "receiver@yahoo.com.vn" };
    vector<string> ccReceiver = { "receiver2@gmail.com" };
    vector<string> bccReceiver = { "receiver3@whatisthis.org" };

    send_mail(sender, toReceiver, ccReceiver, bccReceiver);

    return 0;
}