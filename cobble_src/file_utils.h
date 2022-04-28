//
// Created by Evgenia Golubeva on 28.04.2022.
//

#ifndef COBBLE_SRC_FILE_UTILS_H
#define COBBLE_SRC_FILE_UTILS_H

#include <string>
#include <vector>
#include <filesystem>

std::filesystem::path joinPath(const std::vector<std::string>& pathParts) {
    std::filesystem::path result{};
    for (auto&& part : pathParts) {
        std::filesystem::path pathPart{part};
        result /= pathPart;
    }
    return result;
}

#endif //COBBLE_SRC_FILE_UTILS_H
