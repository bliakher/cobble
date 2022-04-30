# cobble
Dobble game for one player written in C++


## Rules of the game

The game is played with a deck of cards. On each card there are different symbols. Any two cards have **exactly one** one symbol in common. The objective of the game is to find this common symbol.

In the game there is a deck of cards on the left side and one card on the right side, turned symbol side up. Player must find the common symbol between the two cards and click on it. If player found the correct symbol, the top card from the deck is moved to the right pile, revealing the next card from the deck. Game continues until all cards from the deck are used or until the time limit ends. Time limit for one game is 1 minute.

At the start of the game player has 3 lives. If the player makes a mistake, they lose one life. The card is mot moved until the symbol is found correctly. If the player runs out of lives, game ends.

The player is awarded points for finding the right symbol quickly and penalised for making mistakes.

## Configuration

Pictures used in the game and the amount of symbols per card can be configured.

If you want to have `c` symbols per card:
  - `n = c - 1`
  - `n` must be prime for the game to work
  - you will need to provide `n^2 + n + 1` pictures
  - the game will contain `n^2 + n + 1` different cards

For example, if you want to have 4 symbols per card (4 - 1 = 3 is prime), you will need 13 (3^2 + 3 + 1 = 13) different pictures and the game will have 13 cards.

Ideally, pictures should be in `PNG` format with transparent background.

There are 13 pictures already provided with the game, so it is possible to play 2 versions:
  - 3 symbols per card, 7 cards total
  - 4 symbols per card, 13 cards total

You can configure the game with command line arguments.
- `-i <path>` sets the path to the directory containing pictures for the game
- `-c <number>` sets the number of symbols per card

It is sufficient to run the command only once, your choices are saved for future games. The default values are `-i ./data/pictures -c 4`.

## Build

Project uses CMake to build. To build, run

```
cmake cobble_src -B build
cmake --build build
```

Before running, make sure, the `data` directory is located in working directory of the program.
This can be achieved by running the program from the `cobble_src` directory, or by copying the
data directory to output.

**WINDOWS**: Make sure to copy all DLL files to output directory. Find them in SDL2/lib/
```
copy SDL2\lib\x64\*.dll build\Debug\
```


