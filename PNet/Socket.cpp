#include "Socket.h"
#include <assert.h>

namespace PNet {
	Socket::Socket(IPVersion ipversion, SocketHandler handle):ipversion(ipversion), handle(handle)
	{
		assert(ipversion == IPVersion::IPv4);
	}

	PResult PNet::Socket::Create()
	{
		assert(ipversion == IPVersion::IPv4);

		if (handle != INVALID_SOCKET) {
			return PResult::P_NotYetImplemented;
		}

		handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (handle == INVALID_SOCKET) {
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Sucess) {
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Sucess;
	}

	PResult PNet::Socket::Close()
	{
		if (handle == INVALID_SOCKET) {
			return PResult::P_NotYetImplemented;
		}

		int result = closesocket(handle);
		if (result != 0) {
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		handle = INVALID_SOCKET;

		return PResult::P_Sucess;
	}

	PResult Socket::Bind(IPEndpoint endpoint)
	{
		sockaddr_in addr = endpoint.GetSockaddrIPv4();
		int result = bind(handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0) {
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		return PResult::P_Sucess;
	}

	PResult Socket::Listen(IPEndpoint endpoint, int backlog)
	{
		if (Bind(endpoint) != PResult::P_Sucess) {
			return PResult::P_NotYetImplemented;
		};

		int result = listen(handle, backlog);

		if (result != 0) {
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Sucess;
	}

	PResult Socket::Accept(Socket& outSocket)
	{
		SocketHandler acceptedConnectionHandle = accept(handle, nullptr, nullptr);
		if (acceptedConnectionHandle == INVALID_SOCKET) {
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		outSocket = Socket(IPVersion::IPv4, acceptedConnectionHandle);
		return PResult::P_Sucess;
	}

	PResult Socket::Connect(IPEndpoint endpoint)
	{
		sockaddr_in addr = endpoint.GetSockaddrIPv4();
		int result = connect(handle, (sockaddr*) &addr, sizeof(addr));
		if (result != 0) {
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		return PResult::P_Sucess;
	}

	SocketHandler PNet::Socket::getHandle()
	{
		return SocketHandler();
	}

	IPVersion PNet::Socket::GetIPVersion()
	{
		return IPVersion();
	}
	PResult Socket::SetSocketOption(SocketOption option, BOOL value)
	{
		int result = 0;
		switch (option) {
			case SocketOption::TCP_NoDelay:
				result = setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (const char*) & value, sizeof(value));
				break;
			default:
				return PResult::P_NotYetImplemented;
		}

		if (result != 0) {
			int error = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}

		return PResult::P_Sucess;
	}
}
