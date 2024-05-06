#include <iostream>
#include <PNet\IncludeMe.h>
#include <PNet/IPEndpoint.h>

using namespace PNet;

using namespace std;

class ServerGame {
public:
	Socket socket;
	Socket clientSocket;
	void startGameEngine() {
		std::cout << "Starting Game Engine" << std::endl;
		if (Network::Initialize()) {
			std::cout << "socket initialized" << std::endl;

			if (socket.Create() == PResult::P_Sucess) {
				std::cout << "server socket created" << std::endl;
				if (socket.Listen(IPEndpoint("0.0.0.0", 4790)) == PResult::P_Sucess) {
					std::cout << "Socket listening on port 4790" << std::endl;
					
					acceptClient();
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
		
		return;
	}

	void acceptClient() {
		if (socket.Accept(clientSocket) == PResult::P_Sucess) {
			std::cout << "Accepted Connection" << std::endl;

			//expect initial handshake
			initialHandshake();

			char buffer[256];
			strcpy_s(buffer, "Would you like to join a blackjack table?\0");
			int bytesSent = 0;

			int result = clientSocket.Send(buffer, 256, bytesSent);

			if (result != PResult::P_Sucess) {
				std::cout << "Failed to ask client" << std::endl;
			}
			else {
				char clientResponse[10];
				int bytesReceived = 0;
				int result = clientSocket.Recv(clientResponse,256,bytesReceived);
				if (result != PResult::P_Sucess) {
					std::cout << "Failed to recv client response" << std::endl;
				}
				else {
					std::cout << clientResponse << std::endl;
				}
			}

			socket.Close();
		}
		else {
			std::cerr << "could not accept new connection" << std::endl;
		}
	}

	void initialHandshake() {
		char clientResponse[10];
		int bytesReceived = 0;
		int result = clientSocket.Recv(clientResponse, 10, bytesReceived);
		if (result != PResult::P_Sucess) {
			std::cout << "Failed to recv client handshake" << std::endl;
		}
		else {
			std::cout << clientResponse << std::endl;
		}
	}

	ServerGame() {
	}
};

int main() {
	ServerGame server;
	server.startGameEngine();
	system("pause");
	server.acceptClient();
	system("pause");
	Network::Shutdown();
}
