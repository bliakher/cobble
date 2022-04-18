//
// Created by Evgenia Golubeva on 10.04.2022.
//

#ifndef COBBLE_SRC_PROJECTIVE_PLANE_H
#define COBBLE_SRC_PROJECTIVE_PLANE_H

#include <vector>
#include <iostream>

struct Point {
public:
    int x;
    int y;
    int z;
    Point(int x, int y, int z) : x(x), y(y), z(z) {}
};

std::ostream& operator << (std::ostream &os, const Point &p);

class ProjectivePlane {
public:
    explicit ProjectivePlane(int n) : n_(n) {
        pointCount_ = n * n + n + 1;
    }
    std::vector<std::vector<Point>> Generate();
    std::vector<std::vector<int>> ConvertPointsToIdx(const std::vector<std::vector<Point>>& plane);
    int GetPointCount() {
        return pointCount_;
    }

private:
    int pointCount_;
    int n_;
    std::vector<Point> generatePoints() const;
    int convertToIdx(Point point) const;
    std::vector<Point> findPointsOnLine(const Point& lineEquation, const std::vector<Point>& points) const;

};

#endif //COBBLE_SRC_PROJECTIVE_PLANE_H
