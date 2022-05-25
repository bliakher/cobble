//
// Created by Evgenia Golubeva on 18.04.2022.
//

#ifndef COBBLE_SRC_COBBLE_H
#define COBBLE_SRC_COBBLE_H

#include "image_loader.h"
#include "graphic_utils.h"
#include "screen_components.h"
#include "card.h"

#include <utility>
#include <vector>
#include <map>


/**
 * States of the game
 */
enum GameState {
    Intro,
    Playing,
    Outro,
};

class GameScreen;

/**
 * Class representing the game
 */
class Game {
public:
    /**
     * Constructor
     * @param width Width of the game window
     * @param height Height of the game window
     * @param renderer Renderer used to draw on screen
     * @param imagesPerCard Number of images per card
     * @param images List of images used in the game
     */
    Game(int width, int height, SDL_Renderer* renderer, int imagesPerCard, std::vector<Image> images)
        : Width_(width), Height_(height), Renderer_(renderer), ImagesPerCard_(imagesPerCard), Images_(std::move(images)),
            State_(Intro) {}
    /**
     * State of the game
     */
    GameState State_;
    /**
     * Currently displayed game screen
     */
    std::unique_ptr<GameScreen> Screen_;
    /**
     * Width of the game screen
     */
    int Width_;
    /**
     * Height of the game screen
     */
    int Height_;
    /**
     * Number of images per card
     */
    int ImagesPerCard_;
    /**
     * List of images used in the game
     */
    std::vector<Image> Images_;
    /**
     * Renderer used to draw on screen
     */
    SDL_Renderer* Renderer_;
    SDL_Surface* HeartImage_;
    /**
     * Total number of cards in the card deck
     */
    int CardsTotal_;
    /**
     * Initialize the game parameters, create the Introduction screen
     */
    void Init();
    /**
     * Update current game screen
     */
    void Update();
    /**
     * Draw current game screen
     */
    void Draw() const;
    /**
     * Transition from introduction to gameplay
     */
    void StartPlay();
    /**
     * Start new game, transition from outro to gameplay
     */
    void StartNewGame();
    /**
     * Transition from gameplay to outro screen
     */
    void EndGame();
    /**
     * Decrease the number of lives
     *
     * If number of lives reaches 0, game ends
     */
    void DecreaseLives();
    /**
     * Get the remaining number of lives
     * @return remaining lives
     */
    int GetLives() const;
    /**
     * Get remaining time
     * @return remaining time in milliseconds
     */
    long GetRemainingTime() const;
    /**
     * Update based on solved card
     */
    void MarkSolvedCard();
    /**
     * Get the current amount of points
     * @return points
     */
    int GetPoints() const;
    /**
     * Get the number of cards already solved
     * @return number of cards
     */
    int GetCardsDone() const;
private:
    static const int MAX_POINT_INCREMENT = 100;
    static const int LIVES_AT_START = 3;
    static const int TIME_LIMIT = 60000; // 1 min in milliseconds
    long timeStart_;
    long timeRemaining_;
    long lastUpdateTime_;
    int lives_;
    int points_;
    int cardsDone_;
};

/**
 * Class representing a screen displayed to the player
 */
class GameScreen {
public:
    /**
     * Constructor
     * @param game
     * @param width
     * @param height
     * @param renderer
     */
    GameScreen(Game* game, int width, int height, SDL_Renderer* renderer)
            : Game_(game), Width_(width), Height_(height), Renderer_(renderer) {}
    Game* Game_;
    int Width_;
    int Height_;
    SDL_Renderer* Renderer_;
    virtual void Init() {}
    virtual void Draw() {}
    virtual void UpdateOnClick(int mouseX, int mouseY) {}

protected:
    static constexpr char* fontFile_ = (char*)"./data/assets/BodoniBold.ttf";
    static constexpr SDL_Color yellow_ = SDL_Color{255, 255, 128, 255};
    static constexpr SDL_Color white_ = SDL_Color{255, 255, 255, 255};
    static constexpr SDL_Color black_ = SDL_Color{0, 0, 0, 0};
};

class IntroScreen : public GameScreen {
public:
    IntroScreen(Game* game, int width, int height, SDL_Renderer *renderer)
            : GameScreen(game, width, height, renderer) {}
    void Init() override;
    void Draw() override;
    void UpdateOnClick(int mouseX, int mouseY) override;

private:
    Button startButton_;
};

class PlayScreen : public GameScreen {
public:
    PlayScreen(Game* game, int width, int height, SDL_Renderer *renderer)
            : GameScreen(game, width, height, renderer),
              leftCardCenterX_(0), rightCardCenterX_(0), cardCenterY_(0), cardRadius_(0) {}
    void Draw() override;
    void Init() override;
    void UpdateOnClick(int mouseX, int mouseY) override;

private:
    static const short CARD_PADDING = 25;
    static const int MAX_CARDS_DISPLAYED = 4;
    Deck deck_{};
    RenderedCard leftCard_;
    RenderedCard rightCard_;
    short leftCardCenterX_;
    short rightCardCenterX_;
    short cardCenterY_;
    short cardRadius_;
    std::string result_;
    void prepareNextCard();
    void drawBackground();
    void drawHeader();
    void drawTime();
    void drawLives();
};

class OutroScreen : public GameScreen {
public:
    OutroScreen(Game* game, int width, int height, SDL_Renderer *renderer)
            : GameScreen(game, width, height, renderer) {}
    void Init() override;
    void Draw() override;
    void UpdateOnClick(int mouseX, int mouseY) override;
private:
    static const int BUTTON_MARGIN = 10;
    Button newGameButton_;
    Button exitButton_;
};





#endif //COBBLE_SRC_COBBLE_H
