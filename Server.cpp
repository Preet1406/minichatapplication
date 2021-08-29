#include <iostream>
#include <winsock.h>

using namespace std;

#define PORT 9909

struct sockaddr_in srv;
fd_set fr, fw, fe;
int nMaxFd;
int nSocket;
int nArrClient[5];

void ProcessNewMessage(int nClientSocket){
    cout << endl << "Processing the new message for the client socket: " << nClientSocket;
    char buff[256 + 1] = {0, };
    int nRET = recv(nClientSocket, buff, 256, 0);
    if (nRET < 0) {
        cout << endl << "Something wrong happened. Closing the connection for client";
        closesocket(nClientSocket);
        for (int nIndex = 0; nIndex < 5; nIndex++) {
            if (nArrClient[nIndex] == nClientSocket) {
                nArrClient[nIndex]= 0;
                break;
            }
        }
    }
    else {
        cout << endl <<"The message received from the client is : " << buff;
        //Send the response to the client
        send(nClientSocket, "Processed your request", 23, 0);
        cout << endl << "*******************************************************************";
    }
}

void ProcessTheNewRequest() {
    //New connection request
    if (FD_ISSET(nSocket, &fr)) {
        int nLen = sizeof(struct sockaddr);
        int nClientSocket = accept(nSocket, NULL, &nLen);
        if (nClientSocket > 0) {
            //Put it into the client fd_set
            int nIndex;
            for (nIndex = 0; nIndex < 5; nIndex++) {
                if (nArrClient[nIndex] == 0) {
                    nArrClient[nIndex] = nClientSocket;
                    send(nClientSocket, "Got the connection done successfully", 37, 0);
                    break;
                }
            }
            if (nIndex == 5) {
                cout << endl << "No space for new connection";
            }
        }
    }
    else {
        for (int nIndex = 0; nIndex < 5; nIndex++) {
            if (FD_ISSET(nArrClient[nIndex], &fr)) {
                //Got the new message from the client
                //Just recv new message
                //Just queue that for new works of your server to fulfill the request
                ProcessNewMessage(nArrClient[nIndex]);
            }
        }
    }
}

int main() {
    int nRET = 0;

    //Initialize WSA variables
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        cout << "WSA failed to initialize";
    }
    else {
        cout << "WSA initialized successfully";
    }

    //Initialize the socket

    nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket < 0) {
        cout << endl << "Socket not opened";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
        cout << endl << "The socket opened successfully " << nSocket;
    }

    //Initialize the environment of sockaddr structure

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;
    memset(&(srv.sin_zero), 0, 8);

    //setsockopt

    int nOptVal = 0;
    int nOptLen = sizeof(nOptVal);
    nRET = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOptVal, nOptLen);
    if (!nRET) {
        cout << endl << "The setsockopt call successful.";
    }
    else {
        cout << endl << "The setsockopt call failed.";
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    //Bind the socket to the local port

    nRET = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));
    if (nRET < 0) {
        cout << endl << "Failed to bind to local port";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
        cout << endl << "Successfully bind to local port";
    }

    //Listen to thee request from client (queues the requests)
    nRET = listen(nSocket, 5);
    if (nRET < 0) {
        cout << endl << "Failed to start listen to local port";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
        cout << endl << "Started listening to local port";
    }

    nMaxFd = nSocket;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (1) {

        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);

        FD_SET(nSocket, &fr);
        FD_SET(nSocket, &fe);

        for (int nIndex = 0; nIndex < 5; nIndex++) {
            if (nArrClient[nIndex] != 0){
                FD_SET(nArrClient[nIndex], &fr);
                FD_SET(nArrClient[nIndex], &fe);
            }
        }
        //Keep waiting for new requests and proceed as per the request
        nRET = select(nMaxFd + 1, &fr, &fw, &fe, &tv);

        if (nRET > 0) {
            //When someone connects or communicates with a message over a dedicated connection
            cout << endl << "Data on port...Processing...";
            ProcessTheNewRequest();
        }
        else if (nRET == 0) {
            //No connection or any communication request made or you can say that none of the socket descriptors are ready
            
        }
        else {
            
            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }
}
