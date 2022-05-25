//
// Created by Evgenia Golubeva on 10.04.2022.
//

#ifndef COBBLE_SRC_PROJECTIVE_PLANE_H
#define COBBLE_SRC_PROJECTIVE_PLANE_H

#include <vector>
#include <iostream>

/**
 * Point inside of projective plane
 */
struct Point {
public:
    int x; /** x coordinate */
    int y; /** y coordinate */
    int z; /** z coordinate */
    Point(int x, int y, int z) : x(x), y(y), z(z) {}
};

/**
 * Line in a projective plane represented by indexes of its points
 */
using LineIdx = std::vector<int>;
/**
 * Line in a projective plane represented by its points
 */
using LinePoints = std::vector<Point>;

std::ostream& operator << (std::ostream &os, const Point &p);

/**
 * Finite projective plane
 *
 * Rules:
 * - any 2 points define a line
 * - any 2 lines intersect at exactly 1 point
 *
 * Generating:
 * - n is the order of the plane, n must be prime
 * - we take triples of numbers from Z_n - (x, y, z), excluding (0, 0, 0)
 * - we define equivalency where (x, y, z) ~ (l*x, l*y, l*z) for some l from Z_p
 * - we define points on the plane as representatives of equivalency classes that have a 1 on the last non-zero coordinate
 * - that gives as (n^2 + n + 1) points: (x, y, 1) - n^2, (x, 1, 0) - n, (1, 0, 0) - 1
 * - we represent lines with equation a*x + b*y + c = 0, where (a, b, c) are also representatives of equivalency classes
 * - that gives as (n^2 + n + 1) lines
 * - we define lines as collections of points that fit the line equation
 */
class ProjectivePlane {
public:
    /**
     * @param order order of the plane (n is prime)
     */
    explicit ProjectivePlane(int order) : n_(order) {
        pointCount_ = GetSizeFromOrder(order);
    }
    /**
     * Generate the plane of order n.
     * @return List of lines of the plane represented by points which the lines contain
     */
    std::vector<LinePoints> Generate();
    /**
     * Convert points in plane from coordinates to indexes.
     *
     * Indexes are taken from this ordering of the points:
     * - first (1, 0, 0)
     * - then all points (x, 1, 0) in lexicographic order
     * - then all points (x, y, 0) in lexicographic order
     *
     * @param plane Plane with lines represented by points
     * @return Plane with lines represented by indexes of points
     */
    std::vector<LineIdx> ConvertPointsToIdx(const std::vector<LinePoints> & plane);
    int GetPointCount() const {
        return pointCount_;
    }

    static int GetSizeFromOrder(int order) {
        return order * order + order + 1;
    }

private:
    int pointCount_;
    int n_;
    std::vector<Point> generatePoints() const;
    int convertToIdx(Point point) const;
    std::vector<Point> findPointsOnLine(const Point& lineEquation, const std::vector<Point>& points) const;

};

#endif //COBBLE_SRC_PROJECTIVE_PLANE_H
