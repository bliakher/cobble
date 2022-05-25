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

/**
 * Class representing an image loaded from file.
 */
class Image {
public:
    Image(std::string filePath, std::string name) : FilePath_(std::move(filePath)), Name_(std::move(name)) {}
    /**
     * Path to image file
     */
    std::string FilePath_;
    /**
     * Image name
     */
    std::string Name_;
    /**
     * Surface loaded from file
     */
    SDL_Surface* Surface_;
    /**
     * Loads image from file as surface
     */
    void Load();
    /**
     * Compare images based on their names
     * @param image Image to compare to
     * @return
     */
    bool operator==(const Image& image) const;
};

/**
 * Class for finding and loading images
 */
class ImageLoader {
public:
    explicit ImageLoader(std::string directoryPath, int imageCount)
        : DirectoryPath_(std::move(directoryPath)), imageCount_(imageCount) {}
    /**
     * Path to directory from which images are loaded
     */
    std::string DirectoryPath_;
    /**
     * List of loaded images
     */
    std::vector<Image> Images_{};
    /**
     * Find images in the directory and load them
     */
    void Load();
    /**
     * Load image from the given file as texture
     * @param filePath Path to image file
     * @param renderer Renderer used to create texture
     * @return Loaded texture
     */
    static SDL_Texture* LoadTexture(const std::string& filePath, SDL_Renderer* renderer);
    /**
     * Load image from the given file as surface
     * @param filePath Path to image file
     * @return Loaded surface
     */
    static SDL_Surface* LoadSurface(const std::string& filePath);
private:
    void findImageFiles();
    int imageCount_;

};



#endif //COBBLE_SRC_IMAGE_LOADER_H
