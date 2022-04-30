#include <iostream>
#include <string>
#include <array>
#include <algorithm>	//std::shuffle
#include <random>		//std::time
#include <stdlib.h>

namespace GameConstants
{
	constexpr int valCeiling{ 21 };
	constexpr int valFloor{ 17 };
	constexpr int deckSize{ 52 };
}

namespace Random
{
	std::random_device rd;
	std::seed_seq ss{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
	std::mt19937 mt1(ss);

	int getRand()
	{
		std::uniform_int_distribution<> drawCard{ 0, (GameConstants::deckSize - 1) };
		return drawCard(mt1);
	}
}

enum class CardRanks
{
	c2, c3, c4, c5, c6,
	c7, c8, c9, c10,
	Jack,
	Queen,
	King,
	Ace,
	max_rank,
};

std::string printCardRanks(const CardRanks& myCard)
{
	switch (myCard)
	{
	case CardRanks::c2:			return "2";
	case CardRanks::c3:			return "3";
	case CardRanks::c4:			return "4";
	case CardRanks::c5:			return "5";
	case CardRanks::c6:			return "6";
	case CardRanks::c7:			return "7";
	case CardRanks::c8:			return "8";
	case CardRanks::c9:			return "9";
	case CardRanks::c10:		return "T";
	case CardRanks::Jack:		return "J";
	case CardRanks::Queen:		return "Q";
	case CardRanks::King:		return "K";
	case CardRanks::Ace:		return "A";
	default:
		return "Invalid Card";
	}
}

enum class CardSuits
{
	clubs,
	diamonds,
	hearts,
	spades,
	max_suit,
};

std::string printCardSuits(const CardSuits& myCard)
{
	switch (myCard)
	{
	case CardSuits::clubs:			return "C";
	case CardSuits::diamonds:		return "D";
	case CardSuits::hearts:			return "H";
	case CardSuits::spades:			return "S";
	default:
		return "Invalid Card";
	}
}

struct Cards
{
	CardRanks rank{};
	CardSuits suit{};
};

using cards_t = std::array<Cards, GameConstants::deckSize>;

void printCard(const Cards& myCard)
{
	std::cout << printCardRanks(myCard.rank) << printCardSuits(myCard.suit);
}

void createDeck(cards_t& myDeck)
{
	int k{ 0 };
	for (int suits = 0; suits < static_cast<int>(CardSuits::max_suit); ++suits)
	{
		for (int ranks = 0; ranks < static_cast<int>(CardRanks::max_rank); ++ranks)
		{
			myDeck[k] = { static_cast<CardRanks>(ranks), static_cast<CardSuits>(suits) };
			++k;
		}
	}
}

void printDeck(const cards_t& myDeck)
{
	for (const Cards& card : myDeck)
	{
		std::cout << printCardRanks(card.rank) << printCardSuits(card.suit) << " ";
	}
}

void shuffleDeck(cards_t& myDeck)
{
	static std::mt19937 mt{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	std::shuffle(myDeck.begin(), myDeck.end(), mt);
}

enum class GameStatus
{
	start,
	player_turn,
	dealer_turn,
	compare,
	dealer_win,
	player_win,
	end,
};

int getCardValue(const Cards& myCard)
{
	switch (myCard.rank)
	{
	case CardRanks::c2:		return 2;
	case CardRanks::c3:		return 3;
	case CardRanks::c4:		return 4;
	case CardRanks::c5:		return 5;
	case CardRanks::c6:		return 6;
	case CardRanks::c7:		return 7;
	case CardRanks::c8:		return 8;
	case CardRanks::c9:		return 9;
	case CardRanks::c10:
	case CardRanks::Jack:
	case CardRanks::King:
	case CardRanks::Queen:
		return 10;
	case CardRanks::Ace:	return 11;
	default:
		return 0;
	}
}

int drawCard(const cards_t& myDeck)
{
	return	(getCardValue(myDeck.at(Random::getRand())));
}

void printTotals(int dealer, int player)
{
	std::cout << "Dealer :: " << dealer << '\n';
	std::cout << "Player :: " << player << '\n' << '\n';
}

GameStatus hitStand(const cards_t& myDeck, int& playerTotal, const int& dealerTotal)
{
	while (true)
	{
		std::cout << "Hit / Stand? (h/s) :: ";
		char x{};
		std::cin >> x;
		std::cout << '\n';
		int draw(drawCard(myDeck));
		switch (x)
		{
		case 'h':
			std::cout << "You draw " <<
				draw << " and now has a total of "
				<< (playerTotal += draw) << '\n';
			return GameStatus::player_turn;
		case 's':
			return GameStatus::dealer_turn;
		default:
			std::cout << "Invalid input";
			break;
		}
	}
}

bool playBlackJack(const cards_t& myDeck)
{
	int dealerTotal{ drawCard(myDeck) }; //dealer first draw
	int playerTotal{ drawCard(myDeck) }; //player first draw
	GameStatus stat{ GameStatus::start };
	//Game start
	while (stat == GameStatus::start)
	{
		playerTotal += drawCard(myDeck);	 //player second draw
		stat = GameStatus::player_turn;
	}
	//Player turn, choose hit or stand
	while (stat == GameStatus::player_turn)
	{
		printTotals(dealerTotal, playerTotal);
		if (playerTotal == GameConstants::valCeiling)
		{
			stat = GameStatus::player_win;
			break;
		}
		std::cout << "\tPlayer turn\n";
		//player wins if initial draw is 21
		//else, continue to play the game
		stat = hitStand(myDeck, playerTotal, dealerTotal);
		if (playerTotal > GameConstants::valCeiling)
		{
			std::cout << '\n';
			printTotals(dealerTotal, playerTotal);
			std::cout << "Your card exceed 21\n";
			stat = GameStatus::dealer_win;
			break;
		}
		else if (playerTotal == GameConstants::valCeiling)
		{
			stat = GameStatus::player_win;
			break;
		}
	}
	//Dealer turn, draw card until dealer card > 17 || > 21
	while (stat == GameStatus::dealer_turn)
	{
		std::cout << '\n' << "\tDealer turn\n";
		int temp{ drawCard(myDeck) };
		std::cout << "Dealer draw " << temp
			<< " and now has a total of "
			<< (dealerTotal += temp) << '\n';
		printTotals(dealerTotal, playerTotal);
		if (dealerTotal > GameConstants::valCeiling)
		{
			stat = GameStatus::player_win;
			break;
		}
		else if (dealerTotal > GameConstants::valFloor && dealerTotal <= GameConstants::valCeiling)
		{
			stat = GameStatus::compare;
			break;
		}
	}
	while (stat == GameStatus::compare)
	{
		if (dealerTotal >= playerTotal)
			stat = GameStatus::dealer_win;
		else if (dealerTotal < playerTotal)
			stat = GameStatus::player_win;
	}

	while (stat == GameStatus::player_win)
	{
		std::cout << "\n\tYou win\n";
		return true;
	}

	while (stat == GameStatus::dealer_win)
	{
		std::cout << "\n\tYou loose\n";
		return false;
	}

}

bool rerun()
{
	while (true)
	{
		std::cout << "\nPlay again? y/n :: ";
		char x{};
		std::cin >> x;
		switch (x)
		{
		case 'y':
			std::system("CLS");
			return true;
		case 'n':
			return false;
		default:
			std::cout << "Invalid input\n";
			break;
		}
	}
}

double getBalance()
{
	std::cout << "\nEnter your balance :: ";
	double x{};
	std::cin >> x;
	std::system("CLS");
	return x;
}

double getBet(double balance)
{
	while (true)
	{
		std::cout << "Enter your bet :: ";
		double x{};
		std::cin >> x;
		if (x > balance)
			std::cout << "Bet should not exceed balance!\n";
		else if (x < 0)
			std::cout << "Bet should not be less than 0!\n";
		else
			std::cout << std::endl;
		return x;
	}
}

int main()
{
	double balance{ getBalance() };
	bool replay{ true };
	while (replay)
	{
		std::cout << "\t=======BLACK JACK=======\n" << '\n';
		std::cout << "Balance :: " << balance << '\n' << '\n';
		//Deck preparation
		cards_t myDeck{};
		createDeck(myDeck);
		shuffleDeck(myDeck);
		//Bet preparation
		double bet{ getBet(balance) };
		//Play
		if (playBlackJack(myDeck))
			balance += bet;
		else
			balance -= bet;
		std::cout << "Your remaining balance :: " << balance << '\n';
		if (balance == 0)
		{
			std::cout << "You're now bankrupt!\n";
			break;
		}
		replay = rerun();
	}
	std::cout << "\nYou cashed out " << balance << '\n'
		<< "Thank you for playing!\n";
	return 0;
}