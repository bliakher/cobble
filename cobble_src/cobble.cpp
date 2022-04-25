//
// Created by Evgenia Golubeva on 18.04.2022.
//

#include "cobble.h"

#include "SDL2/SDL2_rotozoom.h"
#include "projective_plane.h"
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <SDL2/SDL2_gfxPrimitives.h>

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


void RenderedCard::Draw(SDL_Renderer *renderer) {
    int imageCount = card_->Images_.size();
    int imageSize = radius_ * 2 / (imageCount * 2);
    int degreeIncrement = 90;
    int radiusIncrement = imageSize / 2;
    int degrees = rand() % 360; // start on random degree, so every card looks different
    int radiusPart = 0;
    for (int i = 0; i < card_->Images_.size(); ++i) {
        Image image = card_->Images_[i];
        double scale = imageSize / (double)image.Surface_->w; // scale factor
        int rotation = rand() % 180; // random angle to rotate
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
        if (mouseX >= rect.x && mouseX <= rect.x + rect.w) {
            if (mouseY >= rect.y && mouseY <= rect.y + rect.h) {
                return make_shared<Image>(card_->Images_[imageIdx]);
            }
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

void Deck::Init(const vector<Image> &images, const vector<vector<int>> &cardTemplates) {
    for (auto&& cardTemplate : cardTemplates) {
        Card card {};
        for (auto&& imgIndex : cardTemplate) {
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
        swap(cards_[i], cards_[j]);
    }
}

shared_ptr<Card> Deck::GetNextCard() {
    int returnIdx = topCardIdx_;
    topCardIdx_++; // ToDo: check if there are cards left
    return make_shared<Card>(cards_[returnIdx]);
}

int Deck::GetRemainingCardsCount() {
    return cards_.size() - topCardIdx_;
}


void GameWindow::Init() {
    ProjectivePlane plane{2};
    auto lines = plane.Generate();
    auto linesIdx = plane.ConvertPointsToIdx(lines);
    ImageLoader loader{"/Users/evgeniagolubeva/cobble/cobble_src/data/pictures"};
    loader.Load(renderer_);

    deck_.Init(loader.Images_, linesIdx);
    deck_.Shuffle();
    int circleWidth = Width_ / 2 - 2 * cardPadding_;
    cardRadius_ = circleWidth / 2;
    leftCardCenterX_ = 2 * cardPadding_ + cardRadius_;
    rightCardCenterX_ = leftCardCenterX_ + circleWidth + cardPadding_;
    cardCenterY_ = Height_ - cardPadding_ - cardRadius_;
    shared_ptr<Card> left = deck_.GetNextCard();
    shared_ptr<Card> right = deck_.GetNextCard();
    leftCard_ = RenderedCard{move(left), leftCardCenterX_, cardCenterY_, cardRadius_};
    rightCard_ = RenderedCard{move(right), rightCardCenterX_, cardCenterY_, cardRadius_};
    result_ = leftCard_.GetCommon(rightCard_);
}

void GameWindow::Draw() {
    drawBackground();

    // draw pictures on the cards
    leftCard_.Draw(renderer_);
    rightCard_.Draw(renderer_);

    SDL_RenderPresent(renderer_);

}

void GameWindow::UpdateOnClick(int mouseX, int mouseY) {
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
    }

}

void GameWindow::prepareNextCard() {
    auto newRight = leftCard_.GetCard();
    auto newLeft = deck_.GetNextCard();
    leftCard_ = RenderedCard{newLeft, leftCardCenterX_, cardCenterY_, cardRadius_};
    rightCard_ = RenderedCard{newRight, rightCardCenterX_, cardCenterY_, cardRadius_};
    result_ = leftCard_.GetCommon(rightCard_);
}

void GameWindow::drawBackground() {
    SDL_SetRenderDrawColor(renderer_, 255, 255, 128, 255); // light yellow
    auto windowRect = SDL_Rect{0, 0, Width_, Height_};
    SDL_RenderFillRect(renderer_, &windowRect); // fill background

    // display outline of the deck under the left card
    int deckCount = deck_.GetRemainingCardsCount();
    int outlineCount = deckCount >= 4 ? 4 : deckCount;
    for (int i = outlineCount; i >= 0; i--) {
        filledCircleRGBA(renderer_, (leftCardCenterX_ - (cardPadding_ / 4) * i), cardCenterY_, cardRadius_, 255, 255, 255, 255); // first white circle
        circleRGBA(renderer_, (leftCardCenterX_ - (cardPadding_ / 4) * i), cardCenterY_, cardRadius_, 0, 0, 0, 255); // first circle black border
    }

    filledCircleRGBA(renderer_, rightCardCenterX_, cardCenterY_, cardRadius_, 255, 255, 255, 255); // second white circle
    circleRGBA(renderer_, rightCardCenterX_, cardCenterY_, cardRadius_, 0, 0, 0, 255); // second circle black border
}
