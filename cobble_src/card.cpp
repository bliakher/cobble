
#include "card.h"
#include "graphic_utils.h"

#include "SDL2_rotozoom.h"
#include "SDL_ttf.h"

using namespace std;

constexpr float PI = 3.14159265;

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
        int imageRotation = rand() % 360; // random angle to rotate image;
        imageRotations_.push_back(imageRotation);
    }
}

void RenderedCard::Draw(SDL_Renderer *renderer) {
    // images are placed in a spiral, starting from the center outward
    int imageCount = card_->Images_.size();
    int imageSize = radius_ / imageCount; // image size is counted so that images can fit side by side in the radius of the card
    int radiusIncrement = imageSize / 2; // how much is image moved outward from the previous one
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
    return nullptr;
}

std::string RenderedCard::GetCommon(const RenderedCard &card) {
    return card_->GetCommon(card.card_);
}

Card* RenderedCard::GetCard() {
    return card_;
}
