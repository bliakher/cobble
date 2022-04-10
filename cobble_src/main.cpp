#include "SDL2/SDL.h"
//#include "SDL2_ttf/SDL_ttf.h"

#include <stdio.h>
#include <cstdio>
#include <string>
#include <sstream>

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480
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

//---------------------------------------------------------------------
//  MAIN
//---------------------------------------------------------------------
int main(int argc, char* args[]) {

    SDL_Window* window = NULL;                      // The window we are rendering to
    SDL_Surface* screenSurface = NULL;              // The surface contained by the window
    SDL_Event wEvent;                               // Enable the Window Event handler...

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "[ERROR] SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf( "[ERROR] Window could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x80, 0x80, 0x80));     // Set a gray background canvas
    SDL_UpdateWindowSurface(window);

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
                case SDL_KEYDOWN:           eQuit = true; break;
                case SDL_MOUSEBUTTONDOWN:   eQuit = true; break;
                case SDL_WINDOWEVENT_CLOSE: eQuit = true; break;
                default:
                    //SDL_Log("Window %d got unknown event %d\n", wEvent.window.windowID, wEvent.window.event);
                    break;
            }
        }
        SDL_Delay(100); // Keep < 500 [ms]
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}