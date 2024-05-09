#include <PNet/IncludeMe.h>
#include <iostream>

using namespace PNet;

using namespace std;

const char *SERVER_IP = "137.112.205.90";

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
	}

	void getInitialHands() {
		std::cout << "Waiting for initial Hands" << std::endl;
		char dBuff[40];
		int bytesReceived = 0;
		int result = socket.Recv(dBuff, 40, bytesReceived);

		if (result != PResult::P_Sucess) {
			std::cout << "Error getting data from server" << std::endl;
		}
		else {
			std::string hands = std::string(dBuff);
			std::cout << "Got primitives: " << hands << std::endl;

			char delimiter = ' ;';
			size_t pos = hands.find(delimiter);
			std::string dealerPrim = hands.substr(0, pos);
			std::string playerPrim = hands.substr(pos + 1);

			vector<CardDeck::Card> dealerHand = CardDeck::Card::primitiveToCards(dealerPrim);
			std::cout << "Dealer Hand:" << CardDeck::Deck::cardsToString(dealerHand.data(), dealerHand.size()) << std::endl;
			int dealerHandValue = CardDeck::Card::calculateHandValue(dealerHand);
			std::cout << "Dealer Hand Value: " << dealerHandValue << std::endl;

			vector<CardDeck::Card> playerHand = CardDeck::Card::primitiveToCards(playerPrim);
			std::cout << "Player Hand:" << CardDeck::Deck::cardsToString(playerHand.data(), playerHand.size()) << std::endl;
			int playerHandValue = CardDeck::Card::calculateHandValue(playerHand);
			std::cout << "Player Hand Value: " << playerHandValue << std::endl;

			playerTurn(playerHand, playerHandValue);
		}
	}

	void playerTurn(vector<CardDeck::Card>& playerHand, int& playerHandValue) {
		while (true) {
			//bust
			if (playerHandValue > 21) {
				submitHandToServer("BUST", playerHandValue);
				return;
			}
			else if (playerHandValue == 21) {
				submitHandToServer("STAND", playerHandValue);
				return;
			}
			else {
				char userInput[10];
				std::cout << "Type 'h' to hit or 's' to stand" << std::endl;
				std::cin >> userInput;
				if (*userInput == 'h') {
					CardDeck::Card card = getHitFromServer();
					playerHand.push_back(card);
					playerHandValue += card.cardValue();
					playerTurn(playerHand, playerHandValue);
					continue;
				}
				else {
					submitHandToServer("STAND", playerHandValue);
					return;
				}
			}
		}
	}

	CardDeck::Card getHitFromServer() {
		std::cout << "TODO: Get card from server" << std::endl;
		return { CardDeck::Suit::HEARTS, CardDeck::Rank::ACE };
	}

	void submitHandToServer(std::string action, int handValue) {
		std::cout << "TODO: Tell server to " << action << std::endl;
	}

	void disconnect() {
		socket.Close();
		Network::Shutdown();
	}

	ClientGame() {
	}
};

enum class Statuses {
	BUST,
	STAND
};

int main() {
	ClientGame newGame;
	newGame.startClientEngine();
	newGame.connectToServer();
	newGame.joinGame();
	newGame.getInitialHands();
	system("pause");
	newGame.disconnect();
	
}