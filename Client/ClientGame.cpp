#include <PNet/IncludeMe.h>
#include <iostream>

using namespace PNet;

using namespace std;

int main() {
	if (Network::Initialize()) {
		std::cout << "socket initialized";
		Socket socket;
		if (socket.Create() == PResult::P_Sucess) {
			std::cout << "client socket created" << std::endl;
			socket.Close();
		}
		else {
		std:cerr << "client socket creation failed" << std::endl;
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;

}