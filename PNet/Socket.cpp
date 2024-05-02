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

	SocketHandler PNet::Socket::getHandle()
	{
		return SocketHandler();
	}

	IPVersion PNet::Socket::GetIPVersion()
	{
		return IPVersion();
	}
}
