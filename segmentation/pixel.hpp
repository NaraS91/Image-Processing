#ifndef PIXEL_HPP
#define PIXEL_HPP
#include <opencv2/core.hpp>
struct Pixel {
    size_t x;
    size_t y;
    cv::Vec3b color;
};

static bool operator==(const Pixel& lhs, const Pixel& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.color == rhs.color;
}
static bool operator!=(const Pixel& lhs, const Pixel& rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.color != rhs.color;
}
#endif