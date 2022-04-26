//
// Created by Evgenia Golubeva on 26.04.2022.
//

#ifndef COBBLE_SRC_SCREEN_COMPONENTS_H
#define COBBLE_SRC_SCREEN_COMPONENTS_H

#include "SDL2/SDL.h"

#include "graphic_utils.h"

#include <string>
#include <utility>


class Button {
public:
    Button() =default;
    Button(int x, int y, int width, int height, std::string text, SDL_Color textColor, SDL_Color buttonColor)
        : outline_({x, y, width, height}), text_(std::move(text)), textColor_(textColor), buttonColor_(buttonColor) {}

    void Draw(SDL_Renderer* renderer);
    bool WasClicked(int mouseX, int mouseY);

private:
    SDL_Rect outline_;
    std::string text_;
    SDL_Color textColor_;
    SDL_Color buttonColor_;

};

#endif //COBBLE_SRC_SCREEN_COMPONENTS_H
