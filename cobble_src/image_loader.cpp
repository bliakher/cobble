//
// Created by Evgenia Golubeva on 18.04.2022.
//

#include "image_loader.h"

#include <string>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

SDL_Surface* ImageLoader::LoadSurface(const std::string& filePath, SDL_Renderer *renderer) {
    return IMG_Load(filePath.c_str());
}

SDL_Texture* ImageLoader::LoadTexture(const string& filePath, SDL_Renderer* renderer) {
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = ImageLoader::LoadSurface(filePath, renderer);
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
//        SDL_FreeSurface(surface);
    }
    return texture;
}

void Image::Load(SDL_Renderer* renderer) {
    Surface_ = ImageLoader::LoadSurface(FilePath_, renderer);
    if (Surface_ == NULL) {
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
            cout << "cannot load image " << image.Name_ << endl;
            throw;
        }
    }

}

void ImageLoader::findImageFiles() {
    for (const auto & entry : fs::directory_iterator(DirectoryPath_)) {
        if (entry.path().extension().string() == ".png") {
            Images_.emplace_back(entry.path().string(), entry.path().stem().string());
        }
    }
}


