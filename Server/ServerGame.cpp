#include <iostream>
#include <PNet\IncludeMe.h>

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
			socket.Close();
		}
		else {
		std:cerr << "server socket creation failed" << std::endl;
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;

}
