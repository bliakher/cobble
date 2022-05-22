//
// Created by Evgenia Golubeva on 18.04.2022.
//

#ifndef COBBLE_SRC_COBBLE_H
#define COBBLE_SRC_COBBLE_H

#include "image_loader.h"
#include "graphic_utils.h"
#include "screen_components.h"

#include <utility>
#include <vector>
#include <map>

/**
 * Playing card containing images of symbols
 */
class Card {
public:
    /**
     * List of images on the card
     */
    std::vector<Image> Images_{};
    /**
     * Add new image to the list of images.
     * @param image Image to add
     */
    void AddImage(const Image& image);
    /**
     * Get the name of the common image between 2 cards.
     * @param card Card to compare
     * @return Name of the common image
     */
    std::string GetCommon(const Card* card);
private:


};

/**
 * Class representing a card rendered to the screen
 */
class RenderedCard {
public:
    RenderedCard() = default;
    /**
     * Rendered card has a circular form.
     * @param card Card to be rendered
     * @param centerX X coordinate of the center of the circle (card)
     * @param centerY Y coordinate of the center of the circle (card)
     * @param radius Circle (card) radius
     */
    RenderedCard(Card* card, int centerX, int centerY, int radius)
        : card_(card), centerX_(centerX), centerY_(centerY), radius_(radius) {}
    /**
     * Initialize card
     */
    void Init();
    /**
     * Render card to a renderer.
     * @param renderer Renderer
     */
    void Draw(SDL_Renderer *renderer);
    /**
     * Get image on card that was clicked from mouse coordinates.
     * @param mouseX X coordinate of the mouse
     * @param mouseY Y coordinate of the mouse
     * @return Clicked image or null (if nothing is clicked)
     */
    Image* GetClickedImage(int mouseX, int mouseY);
    /**
     * Get the name of the common image between 2 cards.
     * @param card Card to compare
     * @return Name of the common image
     */
    std::string GetCommon(const RenderedCard& card);
    /**
     * Get inner card object
     * @return Inner card
     */
    Card* GetCard();
private:
    /**
     * Degree increment for spacing images on cards
     */
    static const int DEGREE_INCREMENT = 90;
    Card* card_;
    int centerX_;
    int centerY_;
    int radius_;
    std::map<int, SDL_Rect> renderedImageBorders_{};
    int startDegree_;
    std::vector<int> imageRotations_{};
    void addImageBorder(int imageIdx, int imagePosX, int imagePosY, int width, int height);
};

/**
 * Deck of game cards
 *
 * In underlying structure, the deck of cards is represented by a projective plane.
 * Each card in the deck is represented by 1 line,
 * the points on that line are the images on the card.
 * This guarantees that every 2 cards have exactly 1 image in common.
 * Order of the plane (n) determines the size of the deck.
 * The deck will contain (n^2 + n + 1) cards with (n + 1) images on each card.
 */
class Deck {
public:
    Deck(): topCardIdx_(0) {}
    /**
     * Initialize deck.
     *
     * Create a projective plane of corresponding size. Use the plane to assign images to cards.
     * @param images List of all images in the game
     * @param imagesPerCard Number of images per card
     */
    void Init(const std::vector<Image>& images, int imagesPerCard);
    /**
     * Shuffle cards.
     *
     * Uses the Fisher–Yates shuffle
     */
    void Shuffle();
    /**
     * Open the top card from the deck
     * @return Card
     */
    Card* GetNextCard();
    /**
     * Get count of remaining (unopened) cards in the deck.
     * @return Number of remaining cards
     */
    int GetRemainingCardsCount();
    /**
     * Get total number of cards (opened and unopened).
     * @return Total number of cards
     */
    int GetTotalCardsCount();
private:
    std::vector<Card> cards_{};
    int topCardIdx_;

};

/**
 * States of the game
 */
enum GameState {
    Intro,
    Playing,
    Outro,
};

class GameScreen;

class Game {
public:
    Game(int width, int height, SDL_Renderer* renderer, int imagesPerCard, std::vector<Image> images)
        : Width_(width), Height_(height), Renderer_(renderer), ImagesPerCard_(imagesPerCard), Images_(images),
            State_(Intro) {}
    GameState State_;
    std::unique_ptr<GameScreen> Screen_;
    GameScreen* ScreenPtr_;
    int Width_;
    int Height_;
    int ImagesPerCard_;
    std::vector<Image> Images_;
    SDL_Renderer* Renderer_;
    SDL_Surface* HeartImage_;
    int CardsTotal_;
    void Init();
    void Update();
    void Draw() const;
    void StartPlay();
    void StartNewGame();
    void EndGame();
    void DecreaseLives();
    int GetLives() const;
    long GetRemainingTime() const;
    void MarkSolvedCard();
    int GetPoints() const;
    int GetCardsDone() const;
private:
    static const int MAX_POINT_INCREMENT = 100;
    long timeStart_;
    long timeRemaining_;
    long lastUpdateTime_;
    int lives_;
    int points_;
    int cardsDone_;
    const int LIVES_AT_START = 3;
    const int TIME_LIMIT = 60000; // 1 min in milliseconds
};

class GameScreen {
public:
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
    static constexpr SDL_Color yellow_ = {.r = 255, .g = 255, .b = 128, .a = 255};
    static constexpr SDL_Color white_ = {.r = 255, .g = 255, .b = 255, .a = 255};
    static constexpr SDL_Color black_ = {.r = 0, .g = 0, .b = 0, .a = 255};
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
    Button newGameButton_;
    Button exitButton_;
};





#endif //COBBLE_SRC_COBBLE_H
