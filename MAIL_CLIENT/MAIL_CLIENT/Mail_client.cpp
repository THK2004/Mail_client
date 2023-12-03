#include "Mail_client.h"

void mail_client(Config configData) {
	vector<string> toReceiver;
	vector<string> ccReceiver;
	vector<string> bccReceiver;
	vector<string> filename;

	string welcomeMes =
		"Welcome to mail client program, please select your option:\n"
		"1. To send mail\n"
		"2. To check mail received\n"
		"3. Exit\n"
		"You choose (1, 2 or 3): ";
	std::cout << welcomeMes;

	bool isLooping = 1;
	while (isLooping) {
		int key = _getch();
		if (key == '1') {
			std::cout << "1\n";
			std::cout << "Enter your email information: (if don't have, press enter)\n";
			std::cout << "(Make sure to add \", \" to seperate your mail recipients,\n"
						 "please note that the code doesn't tolerate any typo otherwise.)\n";

			string allToRecp;
			std::cout << "To: ";
			getline(cin, allToRecp);
			if (!allToRecp.empty()) {
				size_t previousCommaPos = 0;
				size_t commaPos = allToRecp.find(", ");
				while (commaPos != std::string::npos) {
					string toRecp = allToRecp.substr(previousCommaPos, commaPos - previousCommaPos);
					toReceiver.push_back(toRecp);
					previousCommaPos = commaPos + 2;
					commaPos = allToRecp.find(", ", commaPos + 1);
				}
				toReceiver.push_back(allToRecp.substr(previousCommaPos));
			}

			string allCCRecp;
			std::cout << "CC: ";
			getline(cin, allCCRecp);
			if (!allCCRecp.empty()) {
				size_t previousCommaPos = 0;
				size_t commaPos = allCCRecp.find(", ");
				while (commaPos != std::string::npos) {
					string ccRecp = allCCRecp.substr(previousCommaPos, commaPos - previousCommaPos);
					ccReceiver.push_back(ccRecp);
					previousCommaPos = commaPos + 2;
					commaPos = allCCRecp.find(", ", commaPos + 1);
				}
				ccReceiver.push_back(allCCRecp.substr(previousCommaPos));
			}
			
			string allBCCRecp;
			std::cout << "BCC: ";
			getline(cin, allBCCRecp);
			if (!allBCCRecp.empty()) {
				size_t previousCommaPos = 0;
				size_t commaPos = allBCCRecp.find(", ");
				while (commaPos != std::string::npos) {
					string bccRecp = allBCCRecp.substr(previousCommaPos, commaPos - previousCommaPos);
					bccReceiver.push_back(bccRecp);
					previousCommaPos = commaPos + 2;
					commaPos = allBCCRecp.find(", ", commaPos + 1);
				}
				bccReceiver.push_back(allBCCRecp.substr(previousCommaPos));
			}
			
			if (toReceiver.empty() && ccReceiver.empty() && bccReceiver.empty()) {
				std::cout << "No recipient data received.\n\n";
				std::cout << welcomeMes;
				isLooping = 1;
			}
			else {
				string subject;
				std::cout << "Subject: ";
				getline(cin, subject);

				string content;
				std::cout << "(Enter your mail content, end with a line containing only '.')\n";
				std::cout << "Content:\n";
				while (true) {
					string line;
					getline(cin, line);
					if (line == ".") {
						break;
					}
					else {
						line += "\r\n";
						content += line;
					}
				}

				std::cout << "Do you want to attach file(s) to the mail? (1.Yes, 2.No): ";
				while (true) {
					int attFile_key = _getch();
					if (attFile_key == '2') {
						std::cout << "2\n";
						break;
					}
					else if (attFile_key == '1') {
						std::cout << "1\n";
						int numOfAttFile = 0;
						while (true) {
							std::cout << "Enter the number of attached file: ";
							string tmp;
							getline(cin, tmp);
							stringstream ss(tmp);
							if (ss >> numOfAttFile) {
								break;
							}
							else {
								std::cout << "Invalid input.\n";
							}
						}
						std::cout << "(Make sure you put the file wanting to send in MAIL_CLIENT/MAIL_CLIENT/FileToSend)\n";
						string fname;
						for (int i = 0; i < numOfAttFile; i++) {
							std::cout << "Enter file no." << i + 1 << "'s name with extension: ";
							getline(cin, fname);
							filename.push_back(fname);
						}
						break;
					}
				}

				send_mail(
					configData.general.user_name,
					configData.general.user_addr,
					configData.general.smtpServer_port,
					configData.general.server_addr,
					toReceiver,
					ccReceiver,
					bccReceiver,
					filename,
					subject,
					content
				);

				std::cout << "\nMail successfully sent.\n\n";
				std::cout << welcomeMes;
				isLooping = 1;
			}
		}
		else if (key == '2') {
			std::cout << "2\n";

			isLooping = 0;
		}
		else if (key == '3') {
			std::cout << "3\n";
			std::cout << "\nProgram exited\n";
			isLooping = 0;
		}
		else{
			isLooping = 1;
			continue;
		}
	}
}