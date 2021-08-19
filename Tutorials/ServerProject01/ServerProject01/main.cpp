#include <iostream>
#include <string>
#include <WS2tcpip.h>


#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {
	const int bufSize = 4096;
	int port = 54000;
	//intitialise winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &wsData);
	if (wsResult != 0) {
		cerr << "Cant Initialise winsock! Quiting" << endl;
		return;
	}
	//create socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Cant create a socket! Quitting" << endl;
		return;
	}
	//bind an ip address and port to the socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;//could also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//tell winsock the socket is listening
	listen(listening, SOMAXCONN); // maximum connnections possible

	//wait for connection
	sockaddr_in client;
	int clientSize = sizeof(client);	
	
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	if (clientSocket == INVALID_SOCKET) {
		cerr << "Cant create a socket! Quitting" << endl;
		return;
	}
	
	

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST); //same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);
	
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) ==0){
		cout << host << "connected on port " << service << endl;

	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on port" <<
			ntohs(client.sin_port) << endl;
	}

	//close listening socket
	closesocket(listening);


	//while loop: accept and echo message back to client
	char buf[bufSize];
	while (true) {
		ZeroMemory(buf, bufSize);
		//wait for client to send data
		int bytesRecevied = recv (clientSocket, buf, bufSize, 0);
		if (bytesRecevied == SOCKET_ERROR) {
			cerr << "Error in recv(). Quitting" << endl;
			return;
		}

		if (bytesRecevied == 0) {
			cerr << "Client has disconnected" << endl;
			break;
		}
		//echo message back to client
		send(clientSocket, buf, bytesRecevied+1, 0);


		
	}
	//close socket
	closesocket(clientSocket);
	//shutdown winsock
	WSACleanup();

}