#include <PNet/IncludeMe.h>
#include <iostream>

using namespace PNet;

using namespace std;

const char *SERVER_IP = "137.112.207.79";

int main() {
	if (Network::Initialize()) {
		std::cout << "socket initialized" << std::endl;
		Socket socket;
		if (socket.Create() == PResult::P_Sucess) {
			std::cout << "client socket created" << std::endl;
			if (socket.Connect(IPEndpoint(SERVER_IP, 4790)) == PResult::P_Sucess) {
				std::cout << "client connected to server" << std::endl;;
				char buffer[256];
				strcpy_s(buffer, "Hello world from client!\0");
				int bytesSent = 0;
				int result = PResult::P_Sucess;
				while (result == PResult::P_Sucess) {
					result = socket.Send(buffer, 256, bytesSent);
					std::cout << "trying to send data to server..." << std::endl;
					Sleep(500);
				}
			}
			else {
				std::cerr << "client socket creation failed" << std::endl;
			}
			socket.Close();
		}
		else {
			std::cerr << "client socket creation failed" << std::endl;
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;

}