#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;
//why is this bad??

void main() {

	string ipAddress = "127.0.0.1"; //IP ADdress of server
	int port = 54000; // Listening port # on the server

	//Intitialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0) {
		cerr << "Can't Start Winsock, Err #" << wsResult << endl;
		return;
	}

	//Create Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	//Fill int a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	
	//Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR) {
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	
	//Do-While loop to send and receive data
	char buf[4096];
	string userInput;
	do {
		//Prompt the user for some text
		cout << ">";
		getline(cin, userInput);

		if (userInput.size() >0) { // make sure the user has typed in something
			//send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR) {
				//wait for response
				ZeroMemory(buf, 4096);
				int bytesRecieved = recv(sock, buf, 4096, 0);
				if (bytesRecieved > 0) {
					//Echo response to console
					cout << "SERVER> " << string(buf, 0, bytesRecieved) << endl;
				}
			}
		}


	} while(userInput.size() > 0);
	
	//Gracefully close down everything
	closesocket(sock);
	WSACleanup();
}