//
// Created by Evgenia Golubeva on 26.04.2022.
//

#ifndef COBBLE_SRC_GRAPHIC_UTILS_H
#define COBBLE_SRC_GRAPHIC_UTILS_H

#include "SDL2/SDL_ttf.h"

class GraphicUtils {
public:
    static bool IsInRect(const SDL_Rect& rect, int x, int y) {
        if (x >= rect.x && x <= rect.x + rect.w) {
            if (y >= rect.y && y <= rect.y + rect.h) {
                return true;
            }
        }
        return false;
    }
    static TTF_Font* GetFont(int size) {
        TTF_Font* font = TTF_OpenFont("/Users/evgeniagolubeva/cobble/cobble_src/data/assets/BodoniBold.ttf", size);
        if(!font) {
            printf("[ERROR] TTF_OpenFont() Failed with: %s\n", TTF_GetError());
            exit(2);
        }
        return font;
    }
    static SDL_Texture* GetTextTexture(SDL_Renderer* renderer, const char* string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC) {
        TTF_Font* font = GetFont(size);
        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
        //SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, fgC);     // aliased glyphs
        SDL_Surface* textSurface = TTF_RenderText_Shaded(font, string, fgC, bgC);   // anti-aliased glyphs
        TTF_CloseFont(font);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        //        SDL_FreeSurface(textSurface);
        return texture;
    }

    static void DrawText ( SDL_Renderer* renderer, const char* string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC) {
        // Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
        SDL_Texture* texture = GetTextTexture(renderer, string, size, x, y, fgC, bgC);
        SDL_Point textureSize;
        SDL_QueryTexture(texture, NULL, NULL, &textureSize.x, &textureSize.y);
        SDL_Rect textLocation = { x, y, textureSize.x, textureSize.y };
        SDL_RenderCopy(renderer, texture, NULL, &textLocation);
    }

    static void DrawTextCentered(SDL_Renderer* renderer, const char* string, int size, int centerX, int centerY, SDL_Color fgC, SDL_Color bgC) {
        SDL_Texture* texture = GetTextTexture(renderer, string, size, centerX, centerY, fgC, bgC);
        SDL_Point textureSize;
        SDL_QueryTexture(texture, NULL, NULL, &textureSize.x, &textureSize.y);
        int topLeftX = centerX - textureSize.x / 2;
        int topLeftY = centerY - textureSize.y / 2;
        SDL_Rect textLocation = { topLeftX, topLeftY, textureSize.x, textureSize.y };
        SDL_RenderCopy(renderer, texture, NULL, &textLocation);
    }
};

#endif //COBBLE_SRC_GRAPHIC_UTILS_H
