#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <list>

struct Coord
{
    Coord(int r, int c) : r(r), c(c) {}
    int r;
    int c;
};

std::list<cv::Rect2d> scan(const cv::Mat& image, const int expansionStep);

#endif /* SCANNER_HPP */
