#pragma once

#include <WinSock2.h>

namespace PNet {
	class Network {
	public:
		static bool Initialize();
		static void Shutdown();
	};
}