//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//
//#pragma comment (lib, "Ws2_32.lib")
//#include <WinSock2.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//
//#define PORT 80
//
//const char szhost[] = "";
//
//class Server {
//public:
//	int portNum;
//	SOCKET serverSock;
//	SOCKADDR_IN sin;
//	SOCKET clientSock;
//	int crecsize = sizeof(sin);
//
//	Server(int port) {
//		portNum = port;
//	}
//	int serverStartUp() {
//		WSADATA w;
//		if (WSAStartup(MAKEWORD(2, 2), &w) != 0)
//		{
//			std::cout << "failed to initialize WSA";
//			return 0;
//		}
//
//		serverSock = socket(AF_INET, SOCK_STREAM, 0);
//
//		if (serverSock != INVALID_SOCKET) {
//			HOSTENT* host = gethostbyname(szhost);
//
//			if (host == nullptr) {
//				std::cout << "couldnt get host by name";
//			}
//
//			ZeroMemory(&sin, sizeof(sin));
//			sin.sin_port = htons(PORT);
//			sin.sin_family = AF_INET;
//			memcpy(&sin.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(sin.sin_addr.S_un.S_addr));
//
//			return 1;
//		}
//		else {
//			std::cerr << WSAGetLastError() << std::endl;
//			//printError(WSAGetLastError(), __LINE__, __FILE__);
//			return 0;
//		}
//	}
//
//	int waitForClientConnections() {
//		clientSock = accept(serverSock, (SOCKADDR*)&sin, &crecsize);
//
//	}
//
//};