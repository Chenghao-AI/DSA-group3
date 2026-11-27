#pragma once

#include <cstdint>

//一个像素由红、绿、蓝三个 8 位通道组成。
struct Pixel {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
};

static_assert(sizeof(Pixel) == 3, "RGB pixels must occupy three bytes.");

inline bool operator==(Pixel left, Pixel right) {
    return left.r == right.r && left.g == right.g && left.b == right.b;
}
