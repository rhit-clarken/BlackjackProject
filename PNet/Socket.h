#pragma once
#include "SocketHandler.h";
#include "PResult.h";
#include "IPVersion.h";

namespace PNet {

	class Socket {
	public:
		Socket(IPVersion ipversion = IPVersion::IPv4,
			SocketHandler handle = INVALID_SOCKET);
		PResult Create();
		PResult Close();
		SocketHandler getHandle();
		IPVersion GetIPVersion();
	private:
		IPVersion ipversion = IPVersion::IPv4;
		SocketHandler handle = INVALID_SOCKET;
	};
}