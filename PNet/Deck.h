#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

namespace CardDeck {
	enum class Rank {
		ACE = 1,
		TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
		JACK, QUEEN, KING
	};

	enum class Suit {
		HEARTS,
		DIAMONDS,
		CLUBS,
		SPADES
	};

	struct Card {
		Suit suit;
		Rank rank;
		std::string getPrimitive();
		int cardValue() const;
		static int calculateHandValue(const std::vector<CardDeck::Card>& hand);
		std::string rankToPrimitive(Rank rank) const;
		std::string suitToPrimitive(Suit suit) const;
	};

	class Deck {
	public:
		Deck();
		void shuffle();
		Card drawCard();
		static std::string cardToString(const Card& card);
		static std::string cardsToString(const Card cards[], int size);
	private:
		std::vector<Card> cards;
		
	};
	
};