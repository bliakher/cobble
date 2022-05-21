#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL2_gfxPrimitives.h"
#include "cobble.h"

#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <fstream>
#include <vector>

#define SCREEN_WIDTH    1000
#define SCREEN_HEIGHT   600
#define WINDOW_TITLE    "Cobble"


bool isPrime(int number) {
    int divider = 2;
    while (divider < number) {
        if (number % divider == 0) {
            return false;
        }
        divider++;
    }
    return true;
}

class ArgParser {
public:
    std::string ImageDir_;
    int ImagesPerCard_;
    void Parse(std::vector<std::string>& arg) {
        if (arg.size() > 1) {
            if (arg.size() == 3) {
                parseOption(arg[1], arg[2]);
            } else if (arg.size() == 5) {
                parseOption(arg[1], arg[2]);
                parseOption(arg[3], arg[4]);
            } else {
                throw std::invalid_argument("Too many arguments");
            }
        }
        std::string imageConfig = "configure_image_dir.txt";
        if (imageDirSet_) {
            writeToConfigFile(imageConfig, ImageDir_);
        } else {
            ImageDir_ = readFromConfigFile(imageConfig);
        }
        std::string countConfig = "configure_image_count.txt";
        if (imageCountSet_) {
            auto value = std::to_string(ImagesPerCard_);
            writeToConfigFile(countConfig, value);
        } else {
            auto value = readFromConfigFile(countConfig);
            ImagesPerCard_ = std::stoi(value);
        }

        if (!isPrime(ImagesPerCard_ - 1)) {
            throw std::invalid_argument("Number of images on card - 1 must be a prime number.");
        }
    }
    void Help() {
        std::cout << "-i <path> : sets path to the directory with images used in game" << std::endl;
        std::cout << "-c <number> : sets number of images per card" << std::endl;
    }
private:
    bool imageDirSet_ = false;
    bool imageCountSet_ = false;
    std::string configDir_ = "./data/configure/";
    void parseOption(std::string& option, std::string& value) {
        if (option == "-i") {
            ImageDir_ = value;
            imageDirSet_ = true;
        } else if (option == "-c") {
            try {
                ImagesPerCard_ = std::stoi(value);
                imageCountSet_ = true;
            } catch (std::exception) {
                throw std::invalid_argument("Number of images on card (-c) must be integer but was: " + value);
            }
        } else {
            throw std::invalid_argument("Unknown option " + option);
        }
    }
    void writeToConfigFile(std::string& fileName, std::string& value) {
        std::ofstream configFile;
        configFile.open(configDir_ + fileName);
        configFile << value;
        configFile.close();
    }
    std::string readFromConfigFile(std::string& fileName) {
        std::ifstream configFile;
        configFile.open(configDir_ + fileName);
        std::string line;
        if (configFile.good()) {
            std::getline(configFile, line);
        }
        else {
            throw std::invalid_argument("Cannot open file " + fileName);
        }
        return line;
    }
};

//---------------------------------------------------------------------
//  MAIN
//---------------------------------------------------------------------
int main(int argc, char** argv) {

    std::vector<std::string> arg(argv, argv + argc);
    ArgParser parser{};
    parser.Parse(arg);


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

    ImageLoader loader{parser.ImageDir_};
    loader.Load(renderer);

    Game game {SCREEN_WIDTH, SCREEN_HEIGHT, renderer, parser.ImagesPerCard_, loader.Images_};
    game.Init();


    //-----------------------------------------------------
    // Draw the Text
    //-----------------------------------------------------
    if(TTF_Init() == -1) {
        printf("[ERROR] TTF_Init() Failed with: %s\n", TTF_GetError());
        exit(2);
    }


    //-----------------------------------------------------
    // Wait for Events to quit & close the window
    //-----------------------------------------------------
    SDL_Delay(1000);  // Wait 1 sec for greasy fingers
    bool eQuit = false;
    while (!eQuit) {

        game.Update();
        game.Draw();
        while(SDL_PollEvent(&wEvent)) {
            switch (wEvent.type) {
                case SDL_QUIT:
                    eQuit = true;
                    break;
//                case SDL_KEYDOWN:           eQuit = true; break;
                case SDL_MOUSEBUTTONDOWN:
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    game.Screen_->UpdateOnClick(mouseX, mouseY);
                    game.Screen_->Draw();
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    eQuit = true;
                    break;
                default:
//                    SDL_Log("Window %d got unknown event %d\n", wEvent.window.windowID, wEvent.window.event);
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
