//
// Created by Evgenia Golubeva on 26.04.2022.
//

#ifndef COBBLE_SRC_SCREEN_COMPONENTS_H
#define COBBLE_SRC_SCREEN_COMPONENTS_H

#include "SDL.h"

#include "graphic_utils.h"

#include <string>
#include <utility>

/**
 * Button with text
 */
class Button {
public:
    Button() =default;
    /**
     * Button constructor
     * @param x X coordinate of the top left corner
     * @param y Y coordinate of the top left corner
     * @param width Button width
     * @param height Button Height
     * @param text Text on button
     * @param fontFile Path to file with text font
     * @param textColor Color of the text
     * @param buttonColor Color of the button
     */
    Button(int x, int y, int width, int height, std::string text, std::string fontFile, SDL_Color textColor, SDL_Color buttonColor)
        : outline_({x, y, width, height}), text_(std::move(text)), fontFile_(std::move(fontFile)), textColor_(textColor), buttonColor_(buttonColor) {}
    /**
     * Draw button to renderer
     * @param renderer Renderer used to draw
     */
    void Draw(SDL_Renderer* renderer);
    /**
     * Check if button was clicked by mouse coordinates
     * @param mouseX X coordinate of the mouse
     * @param mouseY Y coordinate of the mouse
     * @return true if the button was clicked
     */
    bool WasClicked(int mouseX, int mouseY);

private:
    SDL_Rect outline_;
    std::string text_;
    std::string fontFile_;
    SDL_Color textColor_;
    SDL_Color buttonColor_;

};

#endif //COBBLE_SRC_SCREEN_COMPONENTS_H
