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
std::unique_ptr<Image> RenderedCard::GetClickedImage(int mouseX, int mouseY) {
    for (auto&& borderPair : renderedImageBorders_) {
        int imageIdx = borderPair.first;
        SDL_Rect rect = borderPair.second;
        if (mouseX >= rect.x && mouseX <= rect.x + rect.w) {
            if (mouseY >= rect.y && mouseY <= rect.y + rect.h) {
                return make_unique<Image>(card_->Images_[imageIdx]);
            }
        }
    }
    return NULL;
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

unique_ptr<Card> Deck::GetNextCard() {
    //Card card = cards_[topCardIdx_];
    int returnIdx = topCardIdx_;
    topCardIdx_++; // ToDo: check if there are cards left
    return make_unique<Card>(cards_[returnIdx]);
}

void GameWindow::Init(SDL_Renderer* renderer) {
    ProjectivePlane plane{2};
    auto lines = plane.Generate();
    auto linesIdx = plane.ConvertPointsToIdx(lines);
    ImageLoader loader{"/Users/evgeniagolubeva/cobble/cobble_src/data/pictures"};
    loader.Load(renderer);

    deck_.Init(loader.Images_, linesIdx);
    unique_ptr<Card> left = deck_.GetNextCard();
    unique_ptr<Card> right = deck_.GetNextCard();
    int circleWidth = Width_ / 2 - 2 * cardPadding_;
    cardRadius_ = circleWidth / 2;
    leftCardCenterX_ = cardPadding_ + cardRadius_;
    rightCardCenterX_ = leftCardCenterX_ + circleWidth + 2 * cardPadding_;
    cardCenterY_ = Height_ - cardPadding_ - cardRadius_;
    leftCard_ = RenderedCard{move(left), leftCardCenterX_, cardCenterY_, cardRadius_};
    rightCard_ = RenderedCard{move(right), rightCardCenterX_, cardCenterY_, cardRadius_};
}

void GameWindow::Draw(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 128, 255); // light yellow
    auto windowRect = SDL_Rect{0, 0, Width_, Height_};
    SDL_RenderFillRect(renderer, &windowRect); // fill background

    filledCircleRGBA(renderer, leftCardCenterX_, cardCenterY_, cardRadius_, 255, 255, 255, 255); // first white circle
    circleRGBA(renderer, leftCardCenterX_, cardCenterY_, cardRadius_, 0, 0, 0, 255); // first circle black border

    filledCircleRGBA(renderer, rightCardCenterX_, cardCenterY_, cardRadius_, 255, 255, 255, 255); // second white circle
    circleRGBA(renderer, rightCardCenterX_, cardCenterY_, cardRadius_, 0, 0, 0, 255); // second circle black border

    // draw pictures on the cards
    leftCard_.Draw(renderer);
    rightCard_.Draw(renderer);

    SDL_RenderPresent(renderer);

}

void GameWindow::UpdateOnClick(int mouseX, int mouseY) {
    unique_ptr<Image> image;
    if (mouseX < Width_ / 2) {
        image = leftCard_.GetClickedImage(mouseX, mouseY);
    } else {
        image = rightCard_.GetClickedImage(mouseX, mouseY);
    }
    if (image == NULL) {
        return;
    }

}
