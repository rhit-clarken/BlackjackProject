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
		int cardValue();
	};

	class Deck {
	public:
		Deck();
		void shuffle();
		Card drawCard();
		std::string toString() const;
		static std::string cardToString(const Card& card);
		static std::string cardsToString(const Card cards[], int size);
	private:
		std::vector<Card> cards;
		std::string rankToString(Rank rank) const;
		std::string suitToString(Suit suit) const;
	};
};