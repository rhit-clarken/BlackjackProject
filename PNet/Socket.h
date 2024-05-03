#pragma once
#include "SocketHandler.h";
#include "PResult.h";
#include "IPVersion.h";
#include "SocketOption.h"
#include "IPEndpoint.h"

namespace PNet {

	class Socket {
	public:
		Socket( IPVersion ipversion = IPVersion::IPv4,
				SocketHandler handle = INVALID_SOCKET);
		PResult Create();
		PResult Close();
		PResult Bind(IPEndpoint endpoint);
		PResult Listen(IPEndpoint endpoint, int backlog = 5);
		PResult Accept(Socket& outSocket);
		PResult Connect(IPEndpoint endpoint);
		PResult Send(void* data, int numberOfBytes, int& bytesSent);
		PResult Recv(void* destination, int numberOfBytes, int& bytesReceived);

		SocketHandler getHandle();
		IPVersion GetIPVersion();
	private:
		IPVersion ipversion = IPVersion::IPv4;
		SocketHandler handle = INVALID_SOCKET;
		PResult SetSocketOption(SocketOption option, BOOL value);
	};
}