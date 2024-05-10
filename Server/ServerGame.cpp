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
		int result = clientSocket.Send(hBuff, (int) hBuffSize, bytesSent);
		if (result != PResult::P_Sucess) {
			std::cout << "Failed to ask client" << std::endl;
		}
		else {
			std::cout << "Now wait for client" << std::endl;
		}
	}

	void handleClientActions() {
		int numberOfOpenClientThreads = clientHands.size();
		while (true) {
			char clientRequest[50];
			int result;
			int bytesSent, bytesReceived = 0;

			result = clientSocket.Recv(clientRequest, 50, bytesSent);
			if (result != PResult::P_Sucess) {
				std::cout << "Failed to get client actions" << std::endl;
				return;
			}
			else {
				std::string clientAction = std::string(clientRequest);
				std::cout << "Client wants to: " << clientAction << std::endl;
				if (strcmp(clientRequest, "HIT") == 0) {
					handleHitRequest();
				}
				else if (clientAction.find("STAND") != std::string::npos) {
					int i = clientAction.find(";");
					int clientHandValue = std::stoi(clientAction.substr(i + 1));
					handleStand(clientHandValue);
				}
				else {
					std::cout << "Bust by default" << std::endl;
					handleBust();
				}
			}

		}

	}

	void sendClientGameEndStatus(std::string status, vector<CardDeck::Card> dealerNewCards) {
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
		int result = clientSocket.Send(buff, 50, bytesSent);

		if (result != PResult::P_Sucess) {
			std::cout << "Failed to send client game end status" << std::endl;
		}
	}

private:

	vector<CardDeck::Card> dealerDrawUntil17() {
		vector<CardDeck::Card> cardsDrawn;
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

	void handleStand(int clientHand) {
		vector<CardDeck::Card> newCards = dealerDrawUntil17();
		if (clientHand > 21) {
			handleBust();
		}
		else if (dealerHandValue > 21) {
			sendClientGameEndStatus("WIN", newCards);
		}
		else if (clientHand > dealerHandValue) {
			sendClientGameEndStatus("WIN", newCards);
		}
		else if (clientHand < dealerHandValue) {
			sendClientGameEndStatus("LOSE", newCards);
		}
		else {
			sendClientGameEndStatus("PUSH", newCards);
		}
	}

	void handleBust() {
		std::cout << "Client busts. Send client lose status" << std::endl;
		char buff[50];
		int bytesSent = 0;

		strcpy_s(buff, "LOSE");

		int result = clientSocket.Send(buff, 50, bytesSent);

		if (result != PResult::P_Sucess) {
			std::cout << "Failed to send client new card" << std::endl;
		}
	}

	void handleHitRequest() {
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
		int result = clientSocket.Send(cBuff, (int)cBuffSize, bytesSent);
		if (result != PResult::P_Sucess) {
			std::cout << "Failed to send client new card" << std::endl;
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
	server.handleClientActions();
	system("pause");
	Network::Shutdown();
}
