#include "PortraitPhoto.h"

#include "ImageIO.h"

PortraitPhoto::PortraitPhoto(const std::filesystem::path& filename)
    : original(loadImage(filename)) {
    description = "Portrait Photo";
}

Image PortraitPhoto::render() const {
    //返回一份副本，同一张原图可用于多条装饰链。
    return original;
}
