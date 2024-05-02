#include "Network.h"
#include <iostream>

bool PNet::Network::Initialize() {
	WSADATA wsadata;
	int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (result != 0) {
		std::cerr << "couldnt start socket" << std::endl;
		return false;
	}

	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion != 2)) {
		std::cerr << "winsock version error" << std::endl;
		return false;
	}

	return true;
}

void PNet::Network::Shutdown() {
	WSACleanup();
	return;
}