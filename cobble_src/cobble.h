//
// Created by Evgenia Golubeva on 18.04.2022.
//

#ifndef COBBLE_SRC_COBBLE_H
#define COBBLE_SRC_COBBLE_H

#include "image_loader.h"
#include "graphic_utils.h"

#include <utility>
#include <vector>
#include <map>

class Card {
public:
    std::vector<Image> Images_{};
    void AddImage(const Image& image);
    std::string GetCommon(const std::shared_ptr<Card> card);
private:


};

class RenderedCard {
public:
    RenderedCard() = default;
    RenderedCard(std::shared_ptr<Card> card, int centerX, int centerY, int radius)
        : card_(std::move(card)), centerX_(centerX), centerY_(centerY), radius_(radius) {}
        void Init();
    void Draw(SDL_Renderer *renderer);
    std::shared_ptr<Image> GetClickedImage(int mouseX, int mouseY);
    std::string GetCommon(const RenderedCard& card);
    std::shared_ptr<Card> GetCard();
private:
    std::shared_ptr<Card> card_;
    int centerX_;
    int centerY_;
    int radius_;
    std::map<int, SDL_Rect> renderedImageBorders_{};
    int startDegree_;
    std::vector<int> imageRotations_{};
    void addImageBorder(int imageIdx, int imagePosX, int imagePosY, int width, int height);
};

class Deck {
public:
    Deck(): topCardIdx_(0) {}
    void Init(std::string imageDirectory, SDL_Renderer* renderer);
    void Shuffle();
    std::shared_ptr<Card> GetNextCard();
    int GetRemainingCardsCount();
private:
    std::vector<Card> cards_{};
    int topCardIdx_;

};

enum GameState {
    Intro,
    Playing,
    Outro,
};

class GameScreen;
class IntroScreen;
class PlayScreen;

class Game {
public:
    Game(int width, int height, SDL_Renderer* renderer)
        : Width_(width), Height_(height), Renderer_(renderer),
            State_(Intro) {}
    GameState State_;
    std::unique_ptr<GameScreen> Screen_;
    GameScreen* ScreenPtr_;
    int Width_;
    int Height_;
    SDL_Renderer* Renderer_;
    void Init();
    void Update();
    void Draw();
    void StartPlay();
    void StartNewGame();
    void EndGame();
    void DecreaseLives();
private:
    long timeStart_;
    long timeRemaining_;
    long lastUpdateTime_;
    int lives_;
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
private:
};

class IntroScreen : public GameScreen {
public:
    IntroScreen(Game* game, int width, int height, SDL_Renderer *renderer)
        : GameScreen(game, width, height, renderer) {}
    void Init() override;
    void Draw() override;
    void UpdateOnClick(int mouseX, int mouseY) override;

private:
    SDL_Rect startButton_;
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
    Deck deck_{};
    RenderedCard leftCard_;
    RenderedCard rightCard_;
    const int cardPadding_ = 25;
    int leftCardCenterX_;
    int rightCardCenterX_;
    int cardCenterY_;
    int cardRadius_;
    std::string result_;
    void prepareNextCard();
    void drawBackground();
};

class OutroScreen : public GameScreen {
public:
    OutroScreen(Game* game, int width, int height, SDL_Renderer *renderer)
            : GameScreen(game, width, height, renderer) {}
    void Init() override{}
    void Draw() override{}
    void UpdateOnClick(int mouseX, int mouseY) override{}

private:
    SDL_Rect newGameButton_;
    SDL_Rect exitButton_;
};





#endif //COBBLE_SRC_COBBLE_H
