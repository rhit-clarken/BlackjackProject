#include <PNet/IncludeMe.h>
#include <iostream>

using namespace PNet;

using namespace std;

const char *SERVER_IP = "137.112.204.133";

class ClientGame {
public:
	Socket socket;
	void startClientEngine() {
		std::cout << "Starting Client Engine" << std::endl;
		if (Network::Initialize()) {
			std::cout << "socket initialized" << std::endl;
			if (socket.Create() == PResult::P_Sucess) {
				std::cout << "client socket created" << std::endl;
				connectToServer();
			}
			else {
				std::cout << "client socket creation failed" << std::endl;
			}
		}
		return;
	}
	void connectToServer() {
		if (socket.Connect(IPEndpoint(SERVER_IP, 4790)) == PResult::P_Sucess) {
			std::cout << "client connected to server" << std::endl;

			Sleep(1500);

			int result = 0;
			int bytesSent = 0;
			char handShake[10];
			strcpy_s(handShake, "Hello\0");

			std::cout << "sending handshake" << std::endl;
			result = socket.Send(handShake, 10, bytesSent);
			std::cout << result << std::endl;
			std::cout << "doen sending handshake" << std::endl;
			if (result != PResult::P_Sucess) {
				std::cout << "Error sending initial handshake to the server" << std::endl;
			}
		}
		else {
			std::cout << "client socket connection/handshake failed" << std::endl;
		}
	}

	void joinGame() {
		
		std::cout << "Attempting to join game" << std::endl;
		char buffer[256];
		int bytesReceived = 0;
		int result = socket.Recv(buffer, 256, bytesReceived);
		std::cout << buffer << std::endl;
		if (result != PResult::P_Sucess) {
			std::cout << "Error getting data from server" << std::endl;
		}
		else {
			int bytesSent;
			char userInput[10];
			
			std::cout << "Press 0 to join game; enter 1 to stand" << std::endl;
			std::cin >> userInput;
			result = socket.Send(userInput, 10, bytesSent);
			if (result != PResult::P_Sucess) {
				std::cout << "Error sending data to the server" << std::endl;
			}
		}

		socket.Close();
	}

	ClientGame() {
	}
};


int main() {
	ClientGame newGame;
	newGame.startClientEngine();
	newGame.connectToServer();
	newGame.joinGame();
	system("pause");
	Network::Shutdown();
}