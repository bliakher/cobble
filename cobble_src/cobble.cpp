//
// Created by Evgenia Golubeva on 18.04.2022.
//

#include "SDL2/SDL2_rotozoom.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL2_gfxPrimitives.h"

#include "cobble.h"
#include "projective_plane.h"
#include <iostream>
#include <math.h>
#include <cstdlib>

#define PI 3.14159265

using namespace std;

double toRadians(int degrees) {
    return PI * degrees / 180;
}

void Card::AddImage(const Image& image) {
    Images_.push_back(image);
}

std::string Card::GetCommon(const shared_ptr<Card> card) {
    for (auto&& image1 : Images_) {
        for (auto&& image2 : card->Images_) {
            if (image1.Name_ == image2.Name_) {
                return image1.Name_;
            }
        }
    }
    return "";
}

void RenderedCard::Init() {
    startDegree_ = rand() % 360; // start on random degree, so every card looks different
    for (int i = 0; i < card_->Images_.size(); ++i) {
        int imageRotation = rand() % 180; // random angle to rotate image;
        imageRotations_.push_back(imageRotation);
    }
}

void RenderedCard::Draw(SDL_Renderer *renderer) {
    int imageCount = card_->Images_.size();
    int imageSize = radius_ * 2 / (imageCount * 2);
    int degreeIncrement = 90;
    int radiusIncrement = imageSize / 2;
    int degrees = startDegree_;
    int radiusPart = 0;
    for (int i = 0; i < card_->Images_.size(); ++i) {
        Image image = card_->Images_[i];
        double scale = imageSize / (double)image.Surface_->w; // scale factor
        int rotation = imageRotations_[i];
        auto scaledSurface = rotozoomSurface(image.Surface_, rotation, scale, SMOOTHING_OFF);
        auto texture = SDL_CreateTextureFromSurface(renderer, scaledSurface);
        degrees += degreeIncrement;
        radiusPart += radiusIncrement;
        int x = centerX_ + cos(toRadians(degrees)) * radiusPart; // coordinates of top left corner of image
        int y = centerY_ + sin(toRadians(degrees)) * radiusPart;
        SDL_Rect destinationRect {x, y, scaledSurface->w, scaledSurface->h};
        SDL_RenderCopy(renderer, texture, NULL, &destinationRect);
        // save rendered image border
        int scaledWidth, scaledHeight = 0;
        rotozoomSurfaceSize(image.Surface_->w, image.Surface_->h, rotation, scale, &scaledWidth, &scaledHeight);
        addImageBorder(i, x, y, scaledWidth, scaledHeight);
    }
}

void RenderedCard::addImageBorder(int imageIdx, int imagePosX, int imagePosY, int width, int height) {
    renderedImageBorders_.insert(pair<int, SDL_Rect>(imageIdx, SDL_Rect{imagePosX, imagePosY, width, height}));
}

// returns NULL if no image was clicked
std::shared_ptr<Image> RenderedCard::GetClickedImage(int mouseX, int mouseY) {
    for (auto&& borderPair : renderedImageBorders_) {
        int imageIdx = borderPair.first;
        SDL_Rect rect = borderPair.second;
        if (GraphicUtils::IsInRect(rect, mouseX, mouseY)) {
            return make_shared<Image>(card_->Images_[imageIdx]);
        }
    }
    return NULL;
}

std::string RenderedCard::GetCommon(const RenderedCard &card) {
    return card_->GetCommon(card.card_);
}

std::shared_ptr<Card> RenderedCard::GetCard() {
    return card_;
}

void Deck::Init(string imageDirectory, SDL_Renderer* renderer) {
    ProjectivePlane plane{2};
    auto lines = plane.Generate();
    auto linesIdx = plane.ConvertPointsToIdx(lines);
    ImageLoader loader{imageDirectory};
    loader.Load(renderer);

    for (auto&& line : linesIdx) {
        Card card {};
        for (auto&& imgIndex : line) {
            if (imgIndex > loader.Images_.size()) {
                cout << "invalid image index " << imgIndex << " - image count is: " << loader.Images_.size() << endl;
                throw std::exception{};
            }

            card.AddImage(loader.Images_[imgIndex]);
        }
        cards_.push_back(card);
    }
}

void Deck::Shuffle() {
    // Fisher–Yates shuffle
    for (int i = cards_.size() - 1; i >= 1; i--) {
        int j = rand() % (i + 1);
        swap(cards_[i], cards_[j]);
    }
}

shared_ptr<Card> Deck::GetNextCard() {
    if (GetRemainingCardsCount() == 0) {
        return NULL;
    }
    int returnIdx = topCardIdx_;
    topCardIdx_++; // ToDo: check if there are cards left
    return make_shared<Card>(cards_[returnIdx]);
}

int Deck::GetRemainingCardsCount() {
    return cards_.size() - topCardIdx_;
}


void Game::Init() {
    lives_ = LIVES_AT_START;
    Screen_ = make_unique<IntroScreen>(this, Width_, Height_, Renderer_);
    Screen_->Init();
    HeartImage_ = ImageLoader::LoadSurface("/Users/evgeniagolubeva/cobble/cobble_src/data/assets/heart.png", Renderer_);
}

void Game::Update() {
    if (State_ == Intro) {
        return;
    } if (State_ == Playing) {
        long curTime = SDL_GetTicks64();
        long timeSinceLastUpdate = curTime - lastUpdateTime_;
        timeRemaining_ = timeRemaining_ - timeSinceLastUpdate;
        timeRemaining_ = timeRemaining_ >= 0 ? timeRemaining_ : 0;
        lastUpdateTime_ = curTime;
        cout << timeRemaining_ << endl;
        if (timeRemaining_ == 0) {
            EndGame();
        }
    }

}

void Game::Draw() {
    Screen_->Draw();
}

void Game::StartPlay() {
    Screen_ = make_unique<PlayScreen>(this, Width_, Height_, Renderer_);
    Screen_->Init();
    State_ = Playing;
    timeStart_ = SDL_GetTicks64();
    timeRemaining_ = TIME_LIMIT;
    lastUpdateTime_ = timeStart_;
}

void Game::StartNewGame() {
    lives_ = LIVES_AT_START;
    StartPlay();
}

void Game::EndGame() {
    cout << "end of game" << endl;
    Screen_ = make_unique<OutroScreen>(this, Width_, Height_, Renderer_);
    Screen_->Init();
    State_ = Outro;
}

void Game::DecreaseLives() {
    lives_--;
    cout << "decreasing lives to: " << lives_ << endl;
    if (lives_ == 0) {
        EndGame();
    }
}

long Game::GetRemainingTime() {
    return timeRemaining_;
}

int Game::GetLives() {
    return lives_;
}

void PlayScreen::Init() {
    deck_.Init("/Users/evgeniagolubeva/cobble/cobble_src/data/pictures", Renderer_);
    deck_.Shuffle();
    int circleWidth = Width_ / 2 - 2 * padding_;
    cardRadius_ = circleWidth / 2;
    leftCardCenterX_ = 2 * padding_ + cardRadius_;
    rightCardCenterX_ = leftCardCenterX_ + circleWidth + padding_;
    cardCenterY_ = Height_ - padding_ - cardRadius_;
    shared_ptr<Card> left = deck_.GetNextCard();
    shared_ptr<Card> right = deck_.GetNextCard();
    leftCard_ = RenderedCard{move(left), leftCardCenterX_, cardCenterY_, cardRadius_};
    rightCard_ = RenderedCard{move(right), rightCardCenterX_, cardCenterY_, cardRadius_};
    leftCard_.Init();
    rightCard_.Init();
    result_ = leftCard_.GetCommon(rightCard_);
}

void PlayScreen::Draw() {
    drawBackground();

    // draw pictures on the cards
    leftCard_.Draw(Renderer_);
    rightCard_.Draw(Renderer_);

    SDL_RenderPresent(Renderer_);

}

void PlayScreen::UpdateOnClick(int mouseX, int mouseY) {
    shared_ptr<Image> image;
    if (mouseX < Width_ / 2) {
        image = leftCard_.GetClickedImage(mouseX, mouseY);
    } else {
        image = rightCard_.GetClickedImage(mouseX, mouseY);
    }
    if (image == NULL) {
        return;
    }
    cout << "image clicked: " << image->Name_ << endl;
    if (image->Name_ == result_) {
        cout << "result: " << result_ << endl;
        prepareNextCard();
    } else {
        Game_->DecreaseLives();
    }

}

void PlayScreen::prepareNextCard() {
    auto newRight = leftCard_.GetCard();
    auto newLeft = deck_.GetNextCard();
    leftCard_ = RenderedCard{newLeft, leftCardCenterX_, cardCenterY_, cardRadius_};
    rightCard_ = RenderedCard{newRight, rightCardCenterX_, cardCenterY_, cardRadius_};
    leftCard_.Init();
    rightCard_.Init();
    result_ = leftCard_.GetCommon(rightCard_);
}

void PlayScreen::drawBackground() {
    // fill background
    SDL_SetRenderDrawColor(Renderer_, 255, 255, 128, 255); // light yellow
    auto windowRect = SDL_Rect{0, 0, Width_, Height_};
    SDL_RenderFillRect(Renderer_, &windowRect);

    // display outline of the deck under the left card
    int deckCount = deck_.GetRemainingCardsCount();
    int outlineCount = deckCount >= 4 ? 4 : deckCount;
    for (int i = outlineCount; i >= 0; i--) {
        filledCircleRGBA(Renderer_, (leftCardCenterX_ - (padding_ / 4) * i), cardCenterY_, cardRadius_, 255, 255, 255, 255); // first white circle
        circleRGBA(Renderer_, (leftCardCenterX_ - (padding_ / 4) * i), cardCenterY_, cardRadius_, 0, 0, 0, 255); // first circle black border
    }
    // right card
    filledCircleRGBA(Renderer_, rightCardCenterX_, cardCenterY_, cardRadius_, 255, 255, 255, 255); // second white circle
    circleRGBA(Renderer_, rightCardCenterX_, cardCenterY_, cardRadius_, 0, 0, 0, 255); // second circle black border

    drawHeader();
}

void PlayScreen::drawTime() {
    long remainingTime = Game_->GetRemainingTime(); // in milliseconds
    int minutes = remainingTime / 60000;
    int seconds = remainingTime / 1000 - minutes * 60;
    string timeStr = "Remaining time: " + to_string(minutes) + ":" + to_string(seconds);
    SDL_Color black = { 0x0,0x0,0x0 }, yellow = {0xff,0xff, 0x80};
    GraphicUtils::DrawText(Renderer_, timeStr.c_str(), 20, 20, 20, black, yellow);
}

string formatTime(int minSec) {
    string result = to_string(minSec);
    if (minSec < 10) {
        return "0" + result;
    }
    return result;
}

void PlayScreen::drawHeader() {
    SDL_Color black = { 0x0,0x0,0x0 }, yellow = {0xff,0xff, 0x80};

    int textSize = Height_ / 6;
    int headerX = Width_ / 2;
    int headerY = padding_ + textSize / 2;
    string text = "COBBLE";
    GraphicUtils::DrawTextCentered(Renderer_, text.c_str(), textSize, headerX, headerY, black, yellow);

    long remainingTime = Game_->GetRemainingTime(); // in milliseconds
    int minutes = remainingTime / 60000;
    int seconds = remainingTime / 1000 - minutes * 60;
    string timeText = "Remaining time: " + formatTime(minutes) + ":" + formatTime(seconds);
    string livesText = "Lives: ";
    int textTimeX =  padding_;
    int textLivesX = Width_ / 2;
    int textY = headerY + textSize / 2 + padding_;
    GraphicUtils::DrawText(Renderer_, timeText.c_str(), 20, textTimeX, textY, black, yellow);
    GraphicUtils::DrawText(Renderer_, livesText.c_str(), 20, textLivesX, textY, black, yellow);

    int heartPadding = 10;
    int lives = Game_->GetLives();
    double scale = 20 / (double)Game_->HeartImage_->w;
    auto scaled = rotozoomSurface(Game_->HeartImage_, 0, scale, SMOOTHING_OFF);
    auto texture = SDL_CreateTextureFromSurface(Renderer_, scaled);
    for (int i = 0; i < lives; ++i) {
        SDL_Rect destRect{textLivesX + 60 + i* scaled->w + i*heartPadding, textY, scaled->w, scaled->h };
        SDL_RenderCopy(Renderer_, texture, NULL, &destRect);
    }
}

void IntroScreen::Init() {
    int buttonWidth = Width_ / 5;
    int buttonHeight = Height_ / 10;
    int buttonTopLeftX = Width_ / 2 - buttonWidth / 2;
    int buttonTopLeftY = 3 * Height_ / 4;
    SDL_Color white = { 0xff,0xff,0xff }, black = { 0x0,0x0,0x0 };
    startButton_ = {buttonTopLeftX, buttonTopLeftY, buttonWidth, buttonHeight, "Start", black, white};
}

void IntroScreen::Draw() {
    SDL_Color black = { 0x0,0x0,0x0 }, white = { 0xff,0xff,0xff};

    SDL_SetRenderDrawColor(Renderer_, black.r, black.g, black.b, 255); // white
    auto windowRect = SDL_Rect{0, 0, Width_, Height_};
    SDL_RenderFillRect(Renderer_, &windowRect); // fill background

    // cobble header
    int textSize = Height_ / 5;
    int textX = Width_ / 2;
    int textY = Height_ / 3;
    string text = "COBBLE";
    GraphicUtils::DrawTextCentered(Renderer_, text.c_str(), textSize, textX, textY, white, black);

    startButton_.Draw(Renderer_);

    SDL_RenderPresent(Renderer_);
}

void IntroScreen::UpdateOnClick(int mouseX, int mouseY) {
    if (startButton_.WasClicked(mouseX, mouseY)) {
        Game_->StartPlay();
    }
}








