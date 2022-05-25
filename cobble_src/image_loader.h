//
// Created by Evgenia Golubeva on 18.04.2022.
//

#ifndef COBBLE_SRC_IMAGE_LOADER_H
#define COBBLE_SRC_IMAGE_LOADER_H

#include "SDL.h"
#include "SDL_image.h"

#include <string>
#include <utility>
#include <vector>

class Image {
public:
    Image(std::string filePath, std::string name) : FilePath_(std::move(filePath)), Name_(std::move(name)) {}
    std::string FilePath_;
    std::string Name_;
    SDL_Surface* Surface_;
    void Load(SDL_Renderer* renderer);
    bool operator==(const Image& image);
};

class ImageLoader {
public:
    explicit ImageLoader(std::string directoryPath, int imageCount)
        : DirectoryPath_(std::move(directoryPath)), imageCount_(imageCount) {}
    std::string DirectoryPath_;
    std::vector<Image> Images_{};
    void Load(SDL_Renderer* renderer);
    static SDL_Texture* LoadTexture(const std::string& filePath, SDL_Renderer* renderer);
    static SDL_Surface* LoadSurface(const std::string& filePath, SDL_Renderer* renderer);
private:
    void findImageFiles();
    int imageCount_;

};



#endif //COBBLE_SRC_IMAGE_LOADER_H
