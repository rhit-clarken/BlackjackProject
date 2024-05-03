#include "IPEndpoint.h"
#include <WS2tcpip.h>
#include <assert.h>

namespace PNet {
	IPEndpoint::IPEndpoint(const char* ip, unsigned short port)
	{
		this->port = port;

		in_addr addr; //location of ipv4 addr
		int result = inet_pton(AF_INET, ip, &addr);

		if (result == 1) {
			if (addr.S_un.S_addr != INADDR_NONE) {
				ip_string = ip;
				hostname = ip;

				ip_bytes.resize(sizeof(ULONG));
				memcpy(&ip_bytes[0], &addr.S_un.S_addr, sizeof(ULONG));

				ipversion = IPVersion::IPv4;
				return;
			}
		}
	}

	IPVersion PNet::IPEndpoint::GetIPVersion()
	{
		return ipversion;
	}

	std::vector<uint8_t> PNet::IPEndpoint::GetIPBytes()
	{
		return ip_bytes;
	}

	std::string PNet::IPEndpoint::GetHostname()
	{
		return hostname;
	}

	std::string PNet::IPEndpoint::GetIPString()
	{
		return ip_string;
	}

	unsigned short PNet::IPEndpoint::GetPort()
	{
		return this->port;
	}
	sockaddr_in IPEndpoint::GetSockaddrIPv4()
	{
		assert(ipversion == IPVersion::IPv4);
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr, &ip_bytes[0], sizeof(ULONG));
		addr.sin_port = htons(port);

		return addr;
	}
}
