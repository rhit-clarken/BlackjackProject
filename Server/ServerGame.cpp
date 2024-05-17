#include <iostream>
#include <PNet\IncludeMe.h>
#include <PNet/IPEndpoint.h>
#include <thread>


using namespace PNet;
using namespace CardDeck;
using namespace std;

class ServerGame {
public:
	Socket socket;
	//Socket clientSocket;
	CardDeck::Deck deck;
	std::vector<CardDeck::Card> dealerHand;
	int numberOfClients = 0;
	std::vector<vector<CardDeck::Card>> clientHands;
	int dealerHandValue = 0;
	bool inGame = false;
	std::vector<int> clientRecv;

	
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
				//while (1)
				//{
					if (socket.Listen(IPEndpoint("0.0.0.0", 4790)) == PResult::P_Sucess) {
						std::cout << "Socket listening on port 4790" << std::endl;
						//Socket clientSocket;
						////clientSocket.Create();
						///*thread t(&ServerGame::acceptClient, this, clientSocket);
						//t.detach();*/
						////acceptClient();
						////handleClient(&clientSocket);
						//thread t(&ServerGame::handleClient, this, &clientSocket);
						//t.join();
						handleClient();
					
					}
					else {
						std::cerr << "listening on port 4790 failed" << std::endl;
					}
					socket.Close();
				//}
			}
			else {
				std::cerr << "server socket creation failed" << std::endl;
			}
		}
		
		return;
	}

	void handleClient() {
		while (1) {
			Socket clientSocket;
			if (socket.Accept(clientSocket) == PResult::P_Sucess) {
				std::cout << "Accepted Connection" << std::endl;
				thread t(&ServerGame::doit, this, &clientSocket);
				t.join();
				//doit(&clientSocket);
				//clientSocket.Close();
				
			}
			else {
				std::cerr << "could not accept new connection" << std::endl;
			}
		}
	}

	void doit(Socket* clientSocket) {
		acceptClient(clientSocket);
		makeAllInitialHands(clientSocket);
		sendClientsInitialHands(clientSocket);
		handleClientActions(clientSocket);
	}

	void acceptClient(Socket *clientSocket) {
		//if (socket.Accept(*clientSocket) == PResult::P_Sucess) {
		//	std::cout << "Accepted Connection" << std::endl;

			//expect initial handshake
			initialHandshake(*clientSocket);

			char buffer[256];
			strcpy_s(buffer, "Would you like to join a blackjack table?\0");
			int bytesSent = 0;

			int result = clientSocket->Send(buffer, 256, bytesSent);

			if (result != PResult::P_Sucess) {
				std::cout << "Failed to ask client" << std::endl;
			}
			else {
				char clientResponse[10];
				int bytesReceived = 0;
				int result = clientSocket->Recv(clientResponse,256,bytesReceived);
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

			//socket.Close();
		//}
		//else {
		//	std::cerr << "could not accept new connection" << std::endl;
		//}
	}

	void makeAllInitialHands(Socket *clientSocket) {
		//dealer hand
		std::cout << "making dealer hand" << std::endl;
		initialize2CardHand(dealerHand);
		std::cout << CardDeck::Deck::cardsToString(dealerHand.data(), dealerHand.size()) << std::endl;
		dealerHandValue = CardDeck::Card::calculateHandValue(dealerHand);
		std::cout << "value: " << dealerHandValue << std::endl;

		//client hands
		for (int i = 0; i < clientHands.size(); i++) {
			initialize2CardHand(clientHands[i]);
			std::cout << "Client Hand " << i << ":" << std::endl;
			std::cout << CardDeck::Deck::cardsToString(clientHands[i].data(), clientHands[i].size()) << std::endl;
			std::cout << "value: " << CardDeck::Card::calculateHandValue(clientHands[i]) << std::endl;
		}

	}

	void sendClientsInitialHands(Socket *clientSocket) {
		std::string dealerHandPrimitive = "";
		vector<std::string> clientHandPrimitives;
		clientHandPrimitives.reserve(clientHands.size());

		//only want to send client one of dealer cards

		dealerHandPrimitive += dealerHand[0].getPrimitive();


		for (int i = 0; i < clientHands.size(); i++) {
			clientHandPrimitives.push_back("");
			for (int j = 0; j < clientHands[i].size(); j++) {
				clientHandPrimitives[i] += clientHands[i][j].getPrimitive();
			}
		}

		//send dealer hand - TODO: multithread this
		const char* dealerHandBuffer = dealerHandPrimitive.c_str();
		const char* clientHandBuffer = clientHandPrimitives[0].c_str();
		size_t hBuffSize = strlen(dealerHandBuffer) + 1 + strlen(clientHandBuffer) + 1;
		char* hBuff = new char[hBuffSize];
		
		strcpy_s(hBuff, hBuffSize, dealerHandBuffer);
		strcat_s(hBuff, hBuffSize, ";");
		strcat_s(hBuff, hBuffSize, clientHandBuffer);

		int bytesSent = 0;

		std::cout << "sending hand data to client: " << hBuff << std::endl;
		int result = clientSocket->Send(hBuff, (int) hBuffSize, bytesSent);
		if (result != PResult::P_Sucess) {
			std::cout << "Failed to ask client" << std::endl;
		}
		else {
			std::cout << "Now wait for client" << std::endl;
		}
	}

	void handleClientActions(Socket *clientSocket) {
		int numberOfOpenClientThreads = clientHands.size();
		while (true) {
			char clientRequest[50];
			int result;
			int bytesSent, bytesReceived = 0;

			result = clientSocket->Recv(clientRequest, 50, bytesSent);
			if (result != PResult::P_Sucess) {
				std::cout << "Failed to get client actions" << std::endl;
				return;
			}
			else {
				std::string clientAction = std::string(clientRequest);
				std::cout << "Client wants to: " << clientAction << std::endl;
				if (strcmp(clientRequest, "HIT") == 0) {
					handleHitRequest(clientSocket);
				}
				else if (clientAction.find("STAND") != std::string::npos) {
					int i = clientAction.find(";");
					int clientHandValue = std::stoi(clientAction.substr(i + 1));
					handleStand(clientHandValue, clientSocket);
				}
				else {
					std::cout << "Bust by default" << std::endl;
					handleBust(clientSocket);
				}
			}

		}

	}

	void sendClientGameEndStatus(std::string status, vector<CardDeck::Card> dealerNewCards, Socket *clientSocket) {
		std::cout << "Client stands and " << status << std::endl;
		char buff[50];
		int bytesSent = 0;
		std::string newCardsPrimitive = "";

		for (int i = 0; i < dealerNewCards.size(); i++) {
			newCardsPrimitive += dealerNewCards[i].getPrimitive();
		}

		strcpy_s(buff, status.c_str());
		strcat_s(buff, ";");
		strcat_s(buff, newCardsPrimitive.c_str());
		
		std::cout << ". Send client status" << buff << std::endl;
		int result = clientSocket->Send(buff, 50, bytesSent);

		if (result != PResult::P_Sucess) {
			std::cout << "Failed to send client game end status" << std::endl;
		}
	}

private:

	vector<CardDeck::Card> dealerDrawUntil17() {
		std::cout << "DEALER TURN: " << dealerHandValue << std::endl;
		vector<CardDeck::Card> cardsDrawn;
		std::cout << "DEALER FLIPS 2ND CARD TO REVEAL: " << CardDeck::Deck::cardToString(dealerHand[1]) << std::endl;
		cardsDrawn.push_back(dealerHand[1]);
		while (true) {
			if (dealerHandValue > 21) {
				return cardsDrawn;
			}
			else if (dealerHandValue < 17) {
				CardDeck::Card newCard = deck.drawCard();

				int newCardVal = newCard.cardValue();

				cardsDrawn.push_back(newCard);
				dealerHand.push_back(newCard);
				dealerHandValue += newCardVal;

				std::cout << "DEALER DRAWS: " << CardDeck::Deck::cardToString(newCard) << std::endl;
				std::cout << "New dealer hand value: " << dealerHandValue << std::endl;
			}
			else {
				return cardsDrawn;
			}

		}
	}

	void handleStand(int clientHand, Socket *clientSocket) {
		vector<CardDeck::Card> newCards = dealerDrawUntil17();
		if (clientHand > 21) {
			handleBust(clientSocket);
		}
		else if (dealerHandValue > 21) {
			sendClientGameEndStatus("WIN", newCards, clientSocket);
		}
		else if (clientHand > dealerHandValue) {
			sendClientGameEndStatus("WIN", newCards, clientSocket);
		}
		else if (clientHand < dealerHandValue) {
			sendClientGameEndStatus("LOSE", newCards, clientSocket);
		}
		else {
			sendClientGameEndStatus("PUSH", newCards, clientSocket);
		}
	}

	void handleBust(Socket *clientSocket) {
		std::cout << "Client busts. Send client lose status" << std::endl;
		char buff[50];
		int bytesSent = 0;

		strcpy_s(buff, "LOSE");

		int result = clientSocket->Send(buff, 50, bytesSent);

		if (result != PResult::P_Sucess) {
			std::cout << "Failed to send client new card" << std::endl;
		}
	}

	void handleHitRequest(Socket *clientSocket) {
		int bytesSent = 0;

		std::cout << "Need to hit: " << std::endl;
		CardDeck::Card newCard = deck.drawCard();
		std::string newCardPrimitive = newCard.getPrimitive();
		std::cout << "New Card: " << CardDeck::Deck::cardToString(newCard) << std::endl;


		const char* convertBuff = newCardPrimitive.c_str();
		size_t cBuffSize = strlen(convertBuff) + 1;
		char* cBuff = new char[cBuffSize];
		strcpy_s(cBuff, cBuffSize, convertBuff);
		std::cout << "New Card primitive: " << cBuff << std::endl;
		int result = clientSocket->Send(cBuff, (int)cBuffSize, bytesSent);
		if (result != PResult::P_Sucess) {
			std::cout << "Failed to send client new card" << std::endl;
		}
	}

	void initialHandshake(Socket clientSocket) {
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
	//server.acceptClient();
	//server.makeAllInitialHands();
	//server.sendClientsInitialHands();
	//server.handleClientActions();
	system("pause");
	Network::Shutdown();
}
