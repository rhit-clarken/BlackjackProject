#include "Deck.h"
#include <iostream>
#include <sstream>
#include <string>

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


    std::string Card::rankToPrimitive(Rank rank) const {
        switch (rank) {
        case CardDeck::Rank::ACE:   return "1";
        case CardDeck::Rank::TWO:   return "2";
        case CardDeck::Rank::THREE: return "3";
        case CardDeck::Rank::FOUR:  return "4";
        case CardDeck::Rank::FIVE:  return "5";
        case CardDeck::Rank::SIX:   return "6";
        case CardDeck::Rank::SEVEN: return "7";
        case CardDeck::Rank::EIGHT: return "8";
        case CardDeck::Rank::NINE:  return "9";
        case CardDeck::Rank::TEN:   return "10";
        case CardDeck::Rank::JACK:  return "11";
        case CardDeck::Rank::QUEEN: return "12";
        case CardDeck::Rank::KING:  return "13";
        }
        return "";
    }

    std::string Card::suitToPrimitive(Suit suit) const {
        switch (suit) {
        case CardDeck::Suit::HEARTS:   return "1";
        case CardDeck::Suit::DIAMONDS: return "2";
        case CardDeck::Suit::CLUBS:    return "3";
        case CardDeck::Suit::SPADES:   return "4";
        }
        return "";
    }

    std::vector<Card> Card::primitiveToCards(std::string primitive)
    {
        std::vector<Card> cards;
        std::istringstream iss(primitive);
        int primitiveValue;

        while(iss >> primitiveValue) {
            Card thisCard;
            switch (primitiveValue) {
                case 1: thisCard.suit = CardDeck::Suit::HEARTS; break;
                case 2: thisCard.suit = CardDeck::Suit::DIAMONDS; break;
                case 3: thisCard.suit = CardDeck::Suit::CLUBS; break;
                case 4: thisCard.suit = CardDeck::Suit::SPADES; break;
            }

            iss >> primitiveValue;
            switch (primitiveValue) {
                case 1: thisCard.rank = CardDeck::Rank::ACE; break;
                case 2: thisCard.rank = CardDeck::Rank::TWO; break;
                case 3: thisCard.rank = CardDeck::Rank::THREE; break;
                case 4: thisCard.rank = CardDeck::Rank::FOUR; break;
                case 5: thisCard.rank = CardDeck::Rank::FIVE; break;
                case 6: thisCard.rank = CardDeck::Rank::SIX; break;
                case 7: thisCard.rank = CardDeck::Rank::SEVEN; break;
                case 8: thisCard.rank = CardDeck::Rank::EIGHT; break;
                case 9: thisCard.rank = CardDeck::Rank::NINE; break;
                case 10: thisCard.rank = CardDeck::Rank::TEN; break;
                case 11: thisCard.rank = CardDeck::Rank::JACK; break;
                case 12: thisCard.rank = CardDeck::Rank::QUEEN; break;
                case 13: thisCard.rank = CardDeck::Rank::KING; break;
            }
            cards.push_back(thisCard);
        }
        return cards;
    }

    std::string Card::getPrimitive()
    {
        std::string primitive = "";
        primitive = CardDeck::Card::suitToPrimitive(suit) + " " + CardDeck::Card::rankToPrimitive(rank) + " ";
        return primitive;
    }

    int Card::cardValue() const {
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

    int Card::calculateHandValue(const std::vector<CardDeck::Card>& hand)
    {
        int value = 0;
        for (std::vector<CardDeck::Card>::size_type i = 0; i < hand.size(); i++) {
            value += hand[i].cardValue();
        }
        return value;
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
   
}
