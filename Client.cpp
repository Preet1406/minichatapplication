#include <iostream>
#include <winsock.h>
using namespace std;

#define PORT 9909

int nClientSocket;
struct sockaddr_in srv;

int main() {
	int nRET = 0;
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) < 0){
		cout << endl << "WSAStartup failed";
		WSACleanup();
		return (EXIT_FAILURE);
	}
	nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (nClientSocket < 0) {
		cout << endl << "Socket call failed";
		WSACleanup();
		return (EXIT_FAILURE);
	}

	srv.sin_family = AF_INET;
	srv.sin_port = htons(PORT);
	srv.sin_addr.S_un.S_addr = inet_addr("192.168.1.34"); 
	memset(&srv.sin_zero, 0, 8);

	nRET = connect(nClientSocket, (struct sockaddr*)&srv, sizeof(srv));
	if (nRET < 0) {
		cout << endl << "Connection failed";
		WSACleanup();
		return (EXIT_FAILURE);
	}
	else {
		cout << endl << "Connected to the server";
		char buff[255] = { 0, };
		recv(nClientSocket, buff, 255, 0);
		cout << endl << "Just press any key to see the message received from the server";
		getchar();
		cout << endl << buff;
		cout << endl << "Now send your message to the server: ";
		while (1) {
			fgets(buff, 256, stdin);
			send(nClientSocket, buff, 256, 0);
			cout << endl << "Press any key get the response from the server...";
			getchar();
			recv(nClientSocket, buff, 256, 0);
			cout << endl << buff << endl << "Now send the next message:";
		}
	}
}