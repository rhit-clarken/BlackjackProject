#include "Deck.h"


namespace CardDeck {
    CardDeck::Deck::Deck()
    {
        for (int s = static_cast<int>(Suit::HEARTS); s <= static_cast<int>(Suit::SPADES); ++s) {
            for (int r = static_cast<int>(Rank::ACE); r <= static_cast<int>(Rank::KING); ++r) {
                cards.push_back({ static_cast<Suit>(s), static_cast<Rank>(r) });
            }
        }
    }

    void CardDeck::Deck::shuffle()
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }

    CardDeck::Card CardDeck::Deck::drawCard()
    {
        if (!cards.empty()) {
            Card drawnCard = cards.back();
            cards.pop_back();
            return drawnCard;
        }
        else {
            // Handle empty deck scenario
            std::cerr << "Deck is empty!" << std::endl;
            return { Suit::HEARTS, Rank::ACE }; // Dummy card
        }
    }

    int Card::cardValue() {
        switch (rank) {
        case Rank::ACE:
            return 11; // Ace default value is 11
        case Rank::TWO:
            return 2;
        case Rank::THREE:
            return 3;
        case Rank::FOUR:
            return 4;
        case Rank::FIVE:
            return 5;
        case Rank::SIX:
            return 6;
        case Rank::SEVEN:
            return 7;
        case Rank::EIGHT:
            return 8;
        case Rank::NINE:
            return 9;
        case Rank::TEN:
        case Rank::JACK:
        case Rank::QUEEN:
        case Rank::KING:
            return 10; // Face cards have a value of 10
        }
        return 0; // Default case, should never reach here
    }

    std::string CardDeck::Deck::toString() const
    {
        std::string result;
        for (const auto& card : cards) {
            result += rankToString(card.rank) + " of " + suitToString(card.suit) + "\n";
        }
        return result;
    }

    std::string Deck::cardToString(const Card& card)
    {
        std::string result;
        switch (card.rank) {
        case Rank::ACE:   result += "Ace"; break;
        case Rank::TWO:   result += "Two"; break;
        case Rank::THREE: result += "Three"; break;
        case Rank::FOUR:  result += "Four"; break;
        case Rank::FIVE:  result += "Five"; break;
        case Rank::SIX:   result += "Six"; break;
        case Rank::SEVEN: result += "Seven"; break;
        case Rank::EIGHT: result += "Eight"; break;
        case Rank::NINE:  result += "Nine"; break;
        case Rank::TEN:   result += "Ten"; break;
        case Rank::JACK:  result += "Jack"; break;
        case Rank::QUEEN: result += "Queen"; break;
        case Rank::KING:  result += "King"; break;
        }

        result += " of ";

        switch (card.suit) {
            case Suit::HEARTS:   result += "Hearts"; break;
            case Suit::DIAMONDS: result += "Diamonds"; break;
            case Suit::CLUBS:    result += "Clubs"; break;
            case Suit::SPADES:   result += "Spades"; break;
        }

        return result;
    }

    std::string Deck::cardsToString(const Card cards[], int size)
    {
        std::string result;
        for (int i = 0; i < size; ++i) {
            result += cardToString(cards[i]);
            if (i < size - 1) {
                result += ", ";
            }
        }
        return result;
    }


    std::string Deck::rankToString(Rank rank) const {
        switch (rank) {
        case CardDeck::Rank::ACE:   return "Ace";
        case CardDeck::Rank::TWO:   return "Two";
        case CardDeck::Rank::THREE: return "Three";
        case CardDeck::Rank::FOUR:  return "Four";
        case CardDeck::Rank::FIVE:  return "Five";
        case CardDeck::Rank::SIX:   return "Six";
        case CardDeck::Rank::SEVEN: return "Seven";
        case CardDeck::Rank::EIGHT: return "Eight";
        case CardDeck::Rank::NINE:  return "Nine";
        case CardDeck::Rank::TEN:   return "Ten";
        case CardDeck::Rank::JACK:  return "Jack";
        case CardDeck::Rank::QUEEN: return "Queen";
        case CardDeck::Rank::KING:  return "King";
        }
        return "";
    }

    std::string Deck::suitToString(Suit suit) const {
        switch (suit) {
        case CardDeck::Suit::HEARTS:   return "Hearts";
        case CardDeck::Suit::DIAMONDS: return "Diamonds";
        case CardDeck::Suit::CLUBS:    return "Clubs";
        case CardDeck::Suit::SPADES:   return "Spades";
        }
        return "";
    }
   
}
