//
// Created by Evgenia Golubeva on 18.04.2022.
//

#include "SDL2_rotozoom.h"
#include "SDL_ttf.h"
#include "SDL2_gfxPrimitives.h"

#include "cobble.h"
#include "projective_plane.h"
#include <iostream>
#include <math.h>
#include <cstdlib>

constexpr float PI = 3.14159265;

using namespace std;

double toRadians(int degrees) {
    return PI * degrees / 180;
}

void Card::AddImage(const Image& image) {
    Images_.push_back(image);
}

std::string Card::GetCommon(const Card* card) {
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
    // images are place in a spiral, starting from the center outward
    int imageCount = card_->Images_.size();
    int imageSize = radius_ / imageCount; // image size is counted so that images can fit side by side in the radius of the card
    int radiusIncrement = imageSize / 2; // how much image is moved outward from the previous one
    int degrees = startDegree_;
    int radiusPart = 0;
    for (int i = 0; i < card_->Images_.size(); ++i) {
        Image image = card_->Images_[i];
        double scale = imageSize / (double)image.Surface_->w; // scale factor
        int rotation = imageRotations_[i];
        auto scaledSurface = rotozoomSurface(image.Surface_, rotation, scale, SMOOTHING_OFF);
        auto texture = SDL_CreateTextureFromSurface(renderer, scaledSurface);
        degrees += DEGREE_INCREMENT;
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
Image* RenderedCard::GetClickedImage(int mouseX, int mouseY) {
    for (auto&& borderPair : renderedImageBorders_) {
        int imageIdx = borderPair.first;
        SDL_Rect rect = borderPair.second;
        if (GraphicUtils::IsInRect(rect, mouseX, mouseY)) {
            return &card_->Images_[imageIdx];
        }
    }
    return NULL;
}

std::string RenderedCard::GetCommon(const RenderedCard &card) {
    return card_->GetCommon(card.card_);
}

Card* RenderedCard::GetCard() {
    return card_;
}

void Deck::Init(const std::vector<Image>& images, int imagesPerCard) {
    ProjectivePlane plane{imagesPerCard - 1};
    auto lines = plane.Generate();
    auto linesIdx = plane.ConvertPointsToIdx(lines);

    for (auto&& line : linesIdx) {
        Card card {};
        for (auto&& imgIndex : line) {
            if (imgIndex > images.size()) {
                cout << "invalid image index " << imgIndex << " - image count is: " << images.size() << endl;
                throw std::exception{};
            }

            card.AddImage(images[imgIndex]);
        }
        cards_.push_back(card);
    }
}

void Deck::Shuffle() {
    // Fisher–Yates shuffle
    for (int i = cards_.size() - 1; i >= 1; i--) {
        int j = rand() % (i + 1);
        auto card = cards_[i];
        cards_[i] = cards_[j];
        cards_[j] = card;
    }
}

Card* Deck::GetNextCard() {
    if (GetRemainingCardsCount() == 0) {
        return nullptr;
    }
    int returnIdx = topCardIdx_;
    topCardIdx_++;
    return &cards_[returnIdx];
}

int Deck::GetRemainingCardsCount() {
    return cards_.size() - topCardIdx_;
}

int Deck::GetTotalCardsCount() {
    return cards_.size();
}


void Game::Init() {
    lives_ = LIVES_AT_START;
    points_ = 0;
    cardsDone_ = 0;
    CardsTotal_ = 0;
    Screen_ = make_unique<IntroScreen>(this, Width_, Height_, Renderer_);
    Screen_->Init();
    HeartImage_ = ImageLoader::LoadSurface("./data/assets/heart.png", Renderer_);
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

void Game::Draw() const {
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
    points_ = 0;
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

long Game::GetRemainingTime() const {
    return timeRemaining_;
}

int Game::GetLives() const {
    return lives_;
}

int Game::GetPoints() const {
    return points_;
}

void Game::MarkSolvedCard() {
    // update points
    double remainingTimePart = TIME_LIMIT / (double)timeRemaining_;
    int timeBonus = MAX_POINT_INCREMENT * remainingTimePart;
    double remainingLivesPart = LIVES_AT_START / (double)lives_;
    int livesBonus = MAX_POINT_INCREMENT * remainingLivesPart;
    points_ += timeBonus + livesBonus;

    cardsDone_++;
}

int Game::GetCardsDone() const {
    return cardsDone_;
}

void PlayScreen::Init() {
    deck_.Init(Game_->Images_, Game_->ImagesPerCard_);
    deck_.Shuffle();
    Game_->CardsTotal_ = deck_.GetTotalCardsCount();
    int circleWidth = Width_ / 2 - 2 * CARD_PADDING;
    cardRadius_ = circleWidth / 2;
    leftCardCenterX_ = 2 * CARD_PADDING + cardRadius_;
    rightCardCenterX_ = leftCardCenterX_ + circleWidth + CARD_PADDING;
    cardCenterY_ = Height_ - CARD_PADDING - cardRadius_;
    Card* left = deck_.GetNextCard();
    Card* right = deck_.GetNextCard();
    leftCard_ = RenderedCard{left, leftCardCenterX_, cardCenterY_, cardRadius_};
    rightCard_ = RenderedCard{right, rightCardCenterX_, cardCenterY_, cardRadius_};
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
    Image* image;
    if (mouseX < Width_ / 2) {
        image = leftCard_.GetClickedImage(mouseX, mouseY);
    } else {
        image = rightCard_.GetClickedImage(mouseX, mouseY);
    }
    if (image == nullptr) {
        return;
    }
    cout << "image clicked: " << image->Name_ << endl;
    if (image->Name_ == result_) {
        cout << "result: " << result_ << endl;
        Game_->MarkSolvedCard();
        prepareNextCard();
    } else {
        Game_->DecreaseLives();
    }

}

void PlayScreen::prepareNextCard() {
    if (deck_.GetRemainingCardsCount() == 0) {
        Game_->EndGame();
        return;
    }
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
    SDL_SetRenderDrawColor(Renderer_, yellow_.r, yellow_.g, yellow_.b, yellow_.a); // light yellow
    auto windowRect = SDL_Rect{0, 0, Width_, Height_};
    SDL_RenderFillRect(Renderer_, &windowRect);

    // display outline of the deck under the left card
    int deckCount = deck_.GetRemainingCardsCount();
    int outlineCount = deckCount >= MAX_CARDS_DISPLAYED ? MAX_CARDS_DISPLAYED : deckCount;
    int result = 0;
    for (int i = outlineCount; i >= 0; i--) {
        result = filledCircleRGBA(Renderer_, (short)(leftCardCenterX_ - (CARD_PADDING / MAX_CARDS_DISPLAYED) * i), cardCenterY_, cardRadius_, 255, 255, 255, 255); // first white circle
        result = circleRGBA(Renderer_, (short)(leftCardCenterX_ - (CARD_PADDING / MAX_CARDS_DISPLAYED) * i), cardCenterY_, cardRadius_, 0, 0, 0, 255); // first circle black border
    }
    // right card
    filledCircleRGBA(Renderer_, rightCardCenterX_, cardCenterY_, cardRadius_, white_.r, white_.g, white_.b, white_.a); // second white circle
    circleRGBA(Renderer_, rightCardCenterX_, cardCenterY_, cardRadius_, black_.r, black_.g, black_.b, black_.a); // second circle black border

    drawHeader();
}

void PlayScreen::drawTime() {
    long remainingTime = Game_->GetRemainingTime(); // in milliseconds
    int minutes = remainingTime / 60000;
    int seconds = remainingTime / 1000 - minutes * 60;
    string timeStr = "Remaining time: " + to_string(minutes) + ":" + to_string(seconds);
    GraphicUtils::DrawText(fontFile_, Renderer_, timeStr.c_str(), 20, 20, 20, black_, yellow_);
}

string formatTime(int minSec) {
    string result = to_string(minSec);
    if (minSec < 10) {
        return "0" + result;
    }
    return result;
}

void PlayScreen::drawHeader() {
    int textSize = Height_ / 8;
    int headerX = Width_ / 2;
    int headerY = CARD_PADDING / 2 + textSize / 2;
    string text = "COBBLE";
    GraphicUtils::DrawTextCentered(fontFile_, Renderer_, text.c_str(), textSize, headerX, headerY, black_, yellow_);

    long remainingTime = Game_->GetRemainingTime(); // in milliseconds
    int minutes = remainingTime / 60000;
    int seconds = remainingTime / 1000 - minutes * 60;
    string timeText = "Remaining time: " + formatTime(minutes) + ":" + formatTime(seconds);
    string livesText = "Lives: ";
    string pointsText = "Points: " + to_string(Game_->GetPoints());
    int textTimeX =  CARD_PADDING;
    int textLivesX = Width_ / 2;
    int textPointsX = 3*Width_ / 4;
    int textY = headerY + textSize / 2 + CARD_PADDING / 2;
    GraphicUtils::DrawText(fontFile_, Renderer_, timeText.c_str(), 20, textTimeX, textY, black_, yellow_);
    GraphicUtils::DrawTextCentered(fontFile_, Renderer_, livesText.c_str(), 20, textLivesX, textY + 10, black_, yellow_);
    GraphicUtils::DrawText(fontFile_, Renderer_, pointsText.c_str(), 20, textPointsX, textY, black_, yellow_);

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
    startButton_ = {buttonTopLeftX, buttonTopLeftY, buttonWidth, buttonHeight, "Start", fontFile_, black_, white_};
}

void IntroScreen::Draw() {
    SDL_SetRenderDrawColor(Renderer_, black_.r, black_.g, black_.b, black_.a);
    auto windowRect = SDL_Rect{0, 0, Width_, Height_};
    SDL_RenderFillRect(Renderer_, &windowRect); // fill background

    // cobble header
    int textSize = Height_ / 5;
    int textX = Width_ / 2;
    int textY = Height_ / 3;
    string text = "COBBLE";
    GraphicUtils::DrawTextCentered(fontFile_, Renderer_, text.c_str(), textSize, textX, textY, white_, black_);

    startButton_.Draw(Renderer_);

    SDL_RenderPresent(Renderer_);
}

void IntroScreen::UpdateOnClick(int mouseX, int mouseY) {
    if (startButton_.WasClicked(mouseX, mouseY)) {
        Game_->StartPlay();
    }
}


void OutroScreen::Init() {
    int buttonWidth = Width_ / 4;
    int buttonHeight = Height_ / 12;
    int margin = 10;
    int button1TopLeftX = Width_ / 2 - buttonWidth - margin;
    int button2TopLeftX = button1TopLeftX + buttonWidth + 2 * margin;
    int buttonTopLeftY = 3 * Height_ / 4;
    newGameButton_ = {button1TopLeftX, buttonTopLeftY, buttonWidth, buttonHeight, "New Game", fontFile_, yellow_, black_};
    exitButton_ = {button2TopLeftX, buttonTopLeftY, buttonWidth, buttonHeight, "Exit", fontFile_, yellow_, black_};
}

void OutroScreen::Draw() {
    SDL_SetRenderDrawColor(Renderer_, yellow_.r, yellow_.g, yellow_.b, yellow_.a);
    auto windowRect = SDL_Rect{0, 0, Width_, Height_};
    SDL_RenderFillRect(Renderer_, &windowRect); // fill background

    // cobble header
    int headerSize = Height_ / 5;
    int headerX = Width_ / 2;
    int headerY = Height_ / 3;
    string text = "COBBLE";
    GraphicUtils::DrawTextCentered(fontFile_, Renderer_, text.c_str(), headerSize, headerX, headerY, black_, yellow_);

    int textSize = 20;
    int textX = Width_ / 2;
    int textY = Height_ / 2;
    string cards = "Cards solved: " + to_string(Game_->GetCardsDone()) +  " / " + to_string(Game_->CardsTotal_ - 1);
    string points = "Points: " + to_string(Game_->GetPoints());
    GraphicUtils::DrawTextCentered(fontFile_, Renderer_, cards.c_str(), textSize, textX, textY, black_, yellow_);
    GraphicUtils::DrawTextCentered(fontFile_, Renderer_, points.c_str(), textSize, textX, textY + 30, black_, yellow_);

    newGameButton_.Draw(Renderer_);
    exitButton_.Draw(Renderer_);

    SDL_RenderPresent(Renderer_);
}

void OutroScreen::UpdateOnClick(int mouseX, int mouseY) {
    if (newGameButton_.WasClicked(mouseX, mouseY)) {
        Game_->StartNewGame();
    }
    if (exitButton_.WasClicked(mouseX, mouseY)) {
        SDL_Quit();
        exit(0);
    }
}
