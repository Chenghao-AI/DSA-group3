#include "Grayscale.h"

#include <utility>

Grayscale::Grayscale(PhotoPtr wrapped)
    : EffectDecorator(std::move(wrapped)) {}

std::string Grayscale::getDescription() const {
    return photo->getDescription() + ", Grayscale";
}

Image Grayscale::render() const {
    Image img = photo->render();

    //内层先完成处理，再按权重得到灰度值，整数除法向下取整。
    for (auto& p : img.pixels()) {
        const auto y = static_cast<std::uint8_t>(
            (299 * p.r + 587 * p.g + 114 * p.b) / 1000);
        p = {y, y, y};
    }

    return img;
}
