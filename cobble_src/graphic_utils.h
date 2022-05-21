//
// Created by Evgenia Golubeva on 26.04.2022.
//

#ifndef COBBLE_SRC_GRAPHIC_UTILS_H
#define COBBLE_SRC_GRAPHIC_UTILS_H

#include "SDL_ttf.h"
#include <cstdio>
#include <iostream>

/**
 * Class containing multiple utilities for graphics ang text rendering
 */
class GraphicUtils {
public:
    /**
     * Tests if the given point is inside the given rectangle.
     * @param rect Rectangle to be tested
     * @param x X coordinate of the tested point
     * @param y Y coordinate of the tested point
     * @return true if point is inside rectangle
     */
    static bool IsInRect(const SDL_Rect& rect, int x, int y) {
        if (x >= rect.x && x <= rect.x + rect.w) {
            if (y >= rect.y && y <= rect.y + rect.h) {
                return true;
            }
        }
        return false;
    }

    /**
     * Load font from file.
     * @param fontFile File with font in .TTF format
     * @param size Size of the font
     * @return Loaded font object
     */
    static TTF_Font* GetFont(const std::string& fontFile, int size) {
        TTF_Font* font = TTF_OpenFont(fontFile.c_str(), size);
        if(!font) {
            printf("[ERROR] TTF_OpenFont() Failed with: %s\n", TTF_GetError());
            throw std::invalid_argument("Font file " + fontFile + " can't be loaded.");
        }
        return font;
    }

    /**
     * Render text with specific font into texture.
     * @param fontFile File with font in .TTF format
     * @param renderer Renderer used to render text
     * @param string Text to render
     * @param size Size of the font
     * @param fgC Color of the text
     * @param bgC Color of the background
     * @return Rendered text
     */
    static SDL_Texture* GetTextTexture(const std::string& fontFile, SDL_Renderer* renderer, const char* string, int size, SDL_Color fgC, SDL_Color bgC) {
        TTF_Font* font = GetFont(fontFile, size);
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        //SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, fgC);     // aliased glyphs
        SDL_Surface* textSurface = TTF_RenderText_Shaded(font, string, fgC, bgC);   // anti-aliased glyphs
        TTF_CloseFont(font);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        //        SDL_FreeSurface(textSurface);
        return texture;
    }

    /**
     * Render given text into renderer. Position given is the top left corner of the text.
     * @param fontFile File with font in .TTF format
     * @param renderer Renderer used to render text
     * @param string Text to render
     * @param size Size of the font
     * @param x X coordinate of the top left corner of the text
     * @param y Y coordinate of the top left corner of the text
     * @param fgC Color of the text
     * @param bgC Color of the background
     */
    static void DrawText (const std::string& fontFile, SDL_Renderer* renderer, const char* string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC) {
        // Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
        SDL_Texture* texture = GetTextTexture(fontFile, renderer, string, size, fgC, bgC);
        SDL_Point textureSize;
        SDL_QueryTexture(texture, NULL, NULL, &textureSize.x, &textureSize.y);
        SDL_Rect textLocation = { x, y, textureSize.x, textureSize.y };
        SDL_RenderCopy(renderer, texture, NULL, &textLocation);
    }

    /**
     * Render given text into renderer. Text is centered around the given position.
     * @param fontFile File with font in .TTF format
     * @param renderer Renderer used to render text
     * @param string Text to render
     * @param size Size of the font
     * @param centerX X coordinate of the center of the text
     * @param centerY Y coordinate of the center of the text
     * @param fgC Color of the text
     * @param bgC Color of the background
     */
    static void DrawTextCentered(const std::string& fontFile, SDL_Renderer* renderer, const char* string, int size, int centerX, int centerY, SDL_Color fgC, SDL_Color bgC) {
        SDL_Texture* texture = GetTextTexture(fontFile, renderer, string, size, fgC, bgC);
        SDL_Point textureSize;
        SDL_QueryTexture(texture, NULL, NULL, &textureSize.x, &textureSize.y);
        int topLeftX = centerX - textureSize.x / 2;
        int topLeftY = centerY - textureSize.y / 2;
        SDL_Rect textLocation = { topLeftX, topLeftY, textureSize.x, textureSize.y };
        SDL_RenderCopy(renderer, texture, NULL, &textLocation);
    }
};

#endif //COBBLE_SRC_GRAPHIC_UTILS_H
