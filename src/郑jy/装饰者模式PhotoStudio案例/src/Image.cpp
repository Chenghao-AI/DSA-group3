#include "Image.h"

#include <cstddef>
#include <limits>
#include <stdexcept>

Image::Image(int width, int height)
    : width_(width), height_(height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Image width and height must be positive.");
    }

    const auto columns = static_cast<std::size_t>(width);
    const auto rows = static_cast<std::size_t>(height);
    if (columns > std::numeric_limits<std::size_t>::max() / rows) {
        throw std::length_error("Image dimensions are too large.");
    }

    pixels_.resize(columns * rows);
}

Pixel& Image::operator()(int x, int y) {
    return pixels_[static_cast<std::size_t>(y) * width_ + x];
}

const Pixel& Image::operator()(int x, int y) const {
    return pixels_[static_cast<std::size_t>(y) * width_ + x];
}
