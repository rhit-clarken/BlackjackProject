#include <iostream>
#include <PNet\IncludeMe.h>
#include <PNet/IPEndpoint.h>

using namespace PNet;
using namespace CardDeck;
using namespace std;

class ServerGame {
public:
	Socket socket;
	Socket clientSocket;
	CardDeck::Deck deck;
	std::vector<CardDeck::Card> dealerHand;
	int numberOfClients = 0;
	std::vector<vector<CardDeck::Card>> clientHands;
	int dealerHandValue = 0;
	
	ServerGame() {
		deck.shuffle();
		int dealerHandValue = 0;
	}

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
					std::cout << "added client to the game" << std::endl;
					numberOfClients += 1;
					std::cout << "number of clients:" << std::endl;
					std::cout << numberOfClients << std::endl;
					clientHands.resize(numberOfClients);
				}
			}

			socket.Close();
		}
		else {
			std::cerr << "could not accept new connection" << std::endl;
		}
	}

	void makeAllInitialHands() {
		//dealer hand
		std::cout << "making dealer hand" << std::endl;
		initialize2CardHand(dealerHand);
		std::cout << CardDeck::Deck::cardsToString(dealerHand.data(), dealerHand.size()) << std::endl;
		int dealerHandValue = CardDeck::Card::calculateHandValue(dealerHand);
		std::cout << "value: " << dealerHandValue << std::endl;

		//client hands
		for (int i = 0; i < clientHands.size(); i++) {
			initialize2CardHand(clientHands[i]);
			std::cout << "Client Hand " << i << ":" << std::endl;
			std::cout << CardDeck::Deck::cardsToString(clientHands[i].data(), clientHands[i].size()) << std::endl;
			std::cout << "value: " << CardDeck::Card::calculateHandValue(clientHands[i]) << std::endl;
		}

	}

	void sendClientsInitialHands() {
		std::string dealerHandPrimitive = "";
		vector<std::string> clientHandPrimitives;
		clientHandPrimitives.reserve(clientHands.size());


		for (int i = 0; i < dealerHand.size(); i++) {
			dealerHandPrimitive += dealerHand[i].getPrimitive();
		}

		for (int i = 0; i < clientHands.size(); i++) {
			clientHandPrimitives.push_back("");
			for (int j = 0; j < clientHands[i].size(); j++) {
				clientHandPrimitives[i] += clientHands[i][j].getPrimitive();
			}
		}

		//send dealer hand
		const char* dealerHandBuffer = dealerHandPrimitive.c_str();
		size_t dBuffSize = strlen(dealerHandBuffer) + 1;
		char* dBuff = new char[dBuffSize];
		
		strcpy_s(dBuff, dBuffSize, dealerHandBuffer);

		int bytesSent = 0;

		std::cout << "sending hand data to client: " << dBuff << std::endl;
		int result = clientSocket.Send(dBuff, 256, bytesSent);
	}

private:
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

	void initialize2CardHand(vector<CardDeck::Card> & hand) {
		if (hand.size() < 2) {
			hand.resize(2);
		}
		hand[0] = deck.drawCard();
		hand[1] = deck.drawCard();
	}

};

int main() {
	ServerGame server;
	server.startGameEngine();
	server.acceptClient();
	server.makeAllInitialHands();
	server.sendClientsInitialHands();
	system("pause");
	Network::Shutdown();
}
