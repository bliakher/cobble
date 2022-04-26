//
// Created by Evgenia Golubeva on 26.04.2022.
//

#ifndef COBBLE_SRC_GRAPHIC_UTILS_H
#define COBBLE_SRC_GRAPHIC_UTILS_H

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
};

#endif //COBBLE_SRC_GRAPHIC_UTILS_H
