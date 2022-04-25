//
// Created by Evgenia Golubeva on 18.04.2022.
//

#include "image_loader.h"

#include <string>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

SDL_Surface* loadImage(string path, SDL_Surface* screenSurface) {
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

void Image::Load(SDL_Renderer* renderer) {
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = IMG_Load(FilePath_.c_str());
    if (surface) {
        Surface_ = surface;
        texture = SDL_CreateTextureFromSurface(renderer, surface);
//        SDL_FreeSurface(surface);
    }
    Texture_ = texture;
    if (Texture_ == NULL) {
        cout << Name_ << ": texture null" << endl;
        throw std::exception{};
    }
}

// images are identical, if their names are identical
bool Image::operator==(const Image &image) {
    return Name_ == image.Name_;
}

void ImageLoader::Load(SDL_Renderer *renderer) {
    findImageFiles();
    for (auto&& image : Images_) {
        try {
            image.Load(renderer);
        } catch (...) {
            cout << "cannot load images" << endl;
            throw;
        }
    }

}

void ImageLoader::findImageFiles() {
    for (const auto & entry : fs::directory_iterator(DirectoryPath_)) {
        //        cout << entry.path().filename() << endl;
        Images_.emplace_back(entry.path(), entry.path().stem());
    }
}
