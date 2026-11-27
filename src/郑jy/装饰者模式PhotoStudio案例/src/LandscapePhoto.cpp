#include "LandscapePhoto.h"

#include "ImageIO.h"

LandscapePhoto::LandscapePhoto(const std::filesystem::path& filename)
    : original(loadImage(filename)) {
    description = "Landscape Photo";
}

Image LandscapePhoto::render() const {
    //每次渲染都以同一份原始像素为起点。
    return original;
}
