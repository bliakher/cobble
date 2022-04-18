//
// Created by Evgenia Golubeva on 13.04.2022.
//

#include "projective_plane.h"

using namespace std;

std::ostream& operator << (std::ostream &os, const Point &p) {
    return (os << p.x << " " << p.y << " " << p.z  << std::endl);
}

vector<vector<Point>> ProjectivePlane::Generate() {
    // generate points of PP
    vector<Point> points = generatePoints();
    vector<Point> lineEquations = generatePoints();
    vector<vector<Point>> result {};
    for(auto&& lineEq : lineEquations) {
        auto line = findPointsOnLine(lineEq, points);
        result.push_back(line);
    }
    return result;
}

vector<vector<int>> ProjectivePlane::ConvertPointsToIdx(const vector<vector<Point>>& plane) {
    vector<vector<int>> result {};
    for (auto&& line : plane) {
        vector<int> lineConverted {};
        for (auto&& point : line) {
            auto pointConverted = convertToIdx(point);
            lineConverted.push_back(pointConverted);
        }
    }
    return result;
}

vector<Point> ProjectivePlane::findPointsOnLine(const Point& lineEquation, const vector<Point>& points) const {
    vector<Point> line {};
    for(auto&& point : points) {
        // substitute point coordinates into line equation
        int equationResult = (lineEquation.x * point.x + lineEquation.y * point.y + lineEquation.z * point.z) % n_;
        if (equationResult == 0) {
            line.push_back(point);
        }
    }
    return line;
}


std::vector<Point> ProjectivePlane::generatePoints() const {
    vector<Point> points {};
    points.emplace_back(1, 0, 0); // 1 point
    for (int x = 0; x < n_; ++x) {
        points.emplace_back(x, 1, 0); // n points
        for (int y = 0; y < n_; ++y) {
            points.emplace_back(x, y, 1); // n^2 points
        }
    }
    return points;
}

int ProjectivePlane::convertToIdx(Point point) const {
    if (point.y == 0 && point.z == 0) {
        return 0;
    }
    if (point.z == 0) {
        return point.x + point.y;
    }
    return point.x * n_ + point.y + n_ + 1;
}