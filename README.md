# cobble
Dobble game for one player written in C++


## Rules of the game

The game is played with a deck of cards. On each card there different symbols. Any two cards have **exactly one** one symbol in common. The objective of the game is to find this common symbol. 

In the game there is a deck of cards on the left side and one card on the right side, turned symbol side up. Player must find the common symbol between the two cards and click on it. If player found the correct symbol, the top card from the deck is moved to the right pile, revealing the next card from the deck. Game continues until all cards from the deck are used or until the time limit ends. Time limit for one game is 1 minute.

In the start of the game player has 3 lives. If the player makes a mistake, they lose one life. The card is mot moved until the symbol is found correctly. If the player runs out of lives, game ends.

The player is awarded points for finding the right symbol quickly and penalised for making mistakes.

