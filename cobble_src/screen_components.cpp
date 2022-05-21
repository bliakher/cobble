//
// Created by Evgenia Golubeva on 26.04.2022.
//

#include "screen_components.h"

#include "SDL2_gfxPrimitives.h"

void Button::Draw(SDL_Renderer *renderer) {
    // draw button
//    SDL_SetRenderDrawColor(renderer, buttonColor_.r, buttonColor_.g, buttonColor_.b, 255);
//    SDL_RenderFillRect(renderer, &outline_);
    int topRightX = outline_.x + outline_.w;
    int topRightY = outline_.y;
    int bottomLeftX = outline_.x;
    int bottomLeftY = outline_.y + outline_.h;
    roundedBoxRGBA(renderer, topRightX, topRightY, bottomLeftX, bottomLeftY, 10,
                         buttonColor_.r, buttonColor_.g, buttonColor_.b, 255);

    // draw text on button
    int textSize = 3 * outline_.h / 4; // text takes up 3/4 of button height
    int textX = outline_.x + outline_.w / 2;
    int textY = outline_.y + outline_.h / 2;
    GraphicUtils::DrawTextCentered(fontFile_, renderer, text_.c_str(), textSize, textX, textY, textColor_, buttonColor_);
}

bool Button::WasClicked(int mouseX, int mouseY) {
    return GraphicUtils::IsInRect(outline_, mouseX, mouseY);
}

