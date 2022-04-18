#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
//#include "SDL2_ttf/SDL_ttf.h"
#include "SDL2/SDL2_gfxPrimitives.h"

#include <stdio.h>
#include <cstdio>
#include <string>
#include <sstream>
//#include <vector>
//#include <iostream>

#include "projective_plane.h"

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600
#define WINDOW_TITLE    "Hello SDL2!"
#define WINDOW_TEXT     "Hello World!"


// We make a custom stream handler for brievity
inline std::ostream& operator<<(std::ostream& os, SDL_version const& v) {
    os << int(v.major) << '.' << int(v.minor) << '.' << int(v.patch);
    return os;
}

std::string getInfo() {
    SDL_version aa, bb, cc;
    SDL_VERSION(&aa);
    SDL_GetVersion(&bb);
//    SDL_TTF_VERSION(&cc);

    std::ostringstream oss;
    oss << "SDL version  : " << aa << '\n';
    oss << "SDL linker   : " << bb << '\n';
//    oss << "SDL_TTF ver. : " << cc << '\n';
    return oss.str();
}

//void drawText ( SDL_Surface* screen, char* string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC) {
//    // Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
//    TTF_Font* font = TTF_OpenFont("arial.ttf", size);
//    if(!font) {
//        printf("[ERROR] TTF_OpenFont() Failed with: %s\n", TTF_GetError());
//        exit(2);
//    }
//    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
//    //SDL_Surface* textSurface = TTF_RenderText_Solid(font, string, fgC);     // aliased glyphs
//    SDL_Surface* textSurface = TTF_RenderText_Shaded(font, string, fgC, bgC);   // anti-aliased glyphs
//    SDL_Rect textLocation = { x, y, 0, 0 };
//    SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
//    SDL_FreeSurface(textSurface);
//    TTF_CloseFont(font);
//    //printf("[ERROR] Unknown error in drawText(): %s\n", TTF_GetError()); return 1;
//}

SDL_Surface* loadImage(std::string path, SDL_Surface* screenSurface) {
    SDL_Surface* img = IMG_Load(path.c_str());
    if (img == NULL) {
        fprintf(stderr, "could not load image: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Surface* optimizedImg = SDL_ConvertSurface(img, screenSurface->format, 0);
    if (optimizedImg == NULL) fprintf(stderr, "could not optimize image: %s\n", SDL_GetError());
    SDL_FreeSurface(img);
    return optimizedImg;
}

void drawBackGround(SDL_Renderer* renderer, int width, int height) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 128, 255); // light yellow
    auto windowRect = SDL_Rect{0, 0, width, height};
    SDL_RenderFillRect(renderer, &windowRect); // fill background
    int padding = 5;
    int circleWidth = width / 2 - 2 * padding;
    int radius = circleWidth / 2;
    int circle1x = padding + radius;
    int circle2x = circle1x + circleWidth + 2 * padding;
    int circley = height - padding - radius;
    filledCircleRGBA(renderer, circle1x, circley, radius, 255, 255, 255, 255); // first white circle
    circleRGBA(renderer, circle1x, circley, radius, 0, 0, 0, 255); // first circle black border

    filledCircleRGBA(renderer, circle2x, circley, radius, 255, 255, 255, 255); // second white circle
    circleRGBA(renderer, circle2x, circley, radius, 0, 0, 0, 255); // second circle black border

}

//---------------------------------------------------------------------
//  MAIN
//---------------------------------------------------------------------
int main(int argc, char* args[]) {

    SDL_Window* window = NULL;                      // The window we are rendering to
    SDL_Surface* screenSurface = NULL;              // The surface contained by the window
    SDL_Event wEvent;                               // Enable the Window Event handler...
    SDL_Renderer *renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "[ERROR] SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf( "[ERROR] Window could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }
//    screenSurface = SDL_GetWindowSurface(window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        printf ("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        return 3;
    }

//    auto img = loadImage("/Users/evgeniagolubeva/Downloads/statni_znak.png", screenSurface);
//    SDL_BlitSurface(img, NULL, screenSurface, NULL);

    drawBackGround(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderPresent(renderer);
//    SDL_UpdateWindowSurface(window);

    //-----------------------------------------------------
    // Draw the Text
    //-----------------------------------------------------
//    if(TTF_Init() == -1) {
//        printf("[ERROR] TTF_Init() Failed with: %s\n", TTF_GetError());
//        exit(2);
//    }
//
//    SDL_Color fgC1 = { 0xff,0xff,0xff }, bgC1 = {0x00,0x00,0xa0};                               // white text on blue background
//    SDL_Color fgC2 = { 0x00,0x00,0x00 }, bgC2 = {0xff,0x00,0xff};                               // black text on magenta background
//    drawText( screenSurface, (char*) "Hello World! @ (x=50, y=100)", 18,  50,100, fgC1, bgC1);  // 18 pt @ (x=50,y=100)
//    drawText( screenSurface, (char*) "arial.ttf @ (x=200, y=150)",   16, 200,150, fgC2, bgC2);  // 16 pt @ (x=200,y=150)
//
//    int i=0;
//    const int FSIZE = 12;               // Font Size
//    std::string ver = getInfo();
//    const char *pStr = ver.c_str();     // Convert string to char array

    //-----------------------------------------------------
    // Split on '\n' and draw each line further down
    //-----------------------------------------------------
//    char *tok = strtok( (char*) pStr, "\n");
//    while (tok != NULL) {
//        drawText( screenSurface, (char*) tok, FSIZE, 50, (200 + (FSIZE + 4)*i), fgC1, fgC2);
//        tok = strtok(NULL,"\n");
//        i++;
//    }
//
//    SDL_UpdateWindowSurface(window);
//    TTF_Quit();

    //-----------------------------------------------------
    // Wait for Events to quit & close the window
    //-----------------------------------------------------
    SDL_Delay(1000);  // Wait 1 sec for greasy fingers
    bool eQuit = false;
    while (!eQuit) {
        while(SDL_PollEvent(&wEvent)) {
            switch (wEvent.type) {
                case SDL_QUIT:              eQuit = true; break;
//                case SDL_KEYDOWN:           eQuit = true; break;
//                case SDL_MOUSEBUTTONDOWN:   eQuit = true; break;
                case SDL_WINDOWEVENT_CLOSE: eQuit = true; break;
                default:
                    //SDL_Log("Window %d got unknown event %d\n", wEvent.window.windowID, wEvent.window.event);
                    break;
            }
        }
        SDL_Delay(100); // Keep < 500 [ms]
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}