//
// Created by Evgenia Golubeva on 18.04.2022.
//

#ifndef COBBLE_SRC_COBBLE_H
#define COBBLE_SRC_COBBLE_H

#include "image_loader.h"

#include <utility>
#include <vector>
#include <map>

class Card {
public:
    std::vector<Image> Images_{};
    void AddImage(const Image& image);
private:


};

class RenderedCard {
public:
    RenderedCard() = default;
    RenderedCard(std::unique_ptr<Card> card, int centerX, int centerY, int radius)
        : card_(std::move(card)), centerX_(centerX), centerY_(centerY), radius_(radius) {}
    void Draw(SDL_Renderer *renderer);
    std::unique_ptr<Image> GetClickedImage(int mouseX, int mouseY);
private:
    std::unique_ptr<Card> card_;
    int centerX_;
    int centerY_;
    int radius_;
    std::map<int, SDL_Rect> renderedImageBorders_{};
    void addImageBorder(int imageIdx, int imagePosX, int imagePosY, int width, int height);
};

class Deck {
public:
    Deck(): topCardIdx_(0) {}
    void Init(const std::vector<Image>& images, const std::vector<std::vector<int>>& cardTemplates);
    void Shuffle();
    std::unique_ptr<Card> GetNextCard();
private:
    std::vector<Card> cards_{};
    int topCardIdx_;

};

class GameWindow {
public:
    GameWindow(int width, int height)
        : Width_(width), Height_(height), leftCardCenterX_(0), rightCardCenterX_(0), cardCenterY_(0), cardRadius_(0) {}
    void Init(SDL_Renderer* renderer);
    void Draw(SDL_Renderer* renderer);
    void UpdateOnClick(int mouseX, int mouseY);
    int Width_;
    int Height_;
private:
    Deck deck_{};
    RenderedCard leftCard_;
    RenderedCard rightCard_;
    const int cardPadding_ = 10;
    int leftCardCenterX_;
    int rightCardCenterX_;
    int cardCenterY_;
    int cardRadius_;
};






#endif //COBBLE_SRC_COBBLE_H
