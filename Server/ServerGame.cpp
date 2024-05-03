#include <iostream>
#include <PNet\IncludeMe.h>
#include <PNet/IPEndpoint.h>

using namespace PNet;

using namespace std;

class ServerGame {
public:
	int portNum;
	void startGameEngine() {
		cout << "Starting Game Engine";

	}
	ServerGame(int port) {
		portNum = port;
	}
};

int main() {
	if (Network::Initialize()) {
		std::cout << "socket initialized" << std::endl;

		Socket socket;
		if (socket.Create() == PResult::P_Sucess) {
			std::cout << "server socket created" << std::endl;
			if (socket.Listen(IPEndpoint("0.0.0.0", 4790)) == PResult::P_Sucess) {
				std::cout << "Socket listening on port 4790" << std::endl;
				Socket newConnection;
				if (socket.Accept(newConnection) == PResult::P_Sucess) {
					std::cout << "Accepted Connection" << std::endl;

					char buffer[256];
					int bytesReceived = 0;
					int result = PResult::P_Sucess;
					while (result == PResult::P_Sucess) {
						result = newConnection.Recv(buffer, 256, bytesReceived);
						if (result != PResult::P_Sucess) {
							break;
						}
						std::cout << buffer << std::endl;
					}

					socket.Close();
				}
				else {
					std::cerr << "could not accept new connection" << std::endl;
				}
			}
			else {
				std::cerr << "listening on port 4790 failed" << std::endl;
			}
			socket.Close();
		}
		else {
			std::cerr << "server socket creation failed" << std::endl;
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;

}
