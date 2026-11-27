#include "Brightness.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

Brightness::Brightness(PhotoPtr wrapped, int amount)
    : EffectDecorator(std::move(wrapped)), delta(amount) {
    if (delta < -255 || delta > 255) {
        throw std::invalid_argument("Brightness must be between -255 and 255.");
    }
}

std::string Brightness::getDescription() const {
    return photo->getDescription() + ", Brightness";
}

Image Brightness::render() const {
    Image img = photo->render();

    //已有的水印和相框也是图像像素，都会参与本次调节。
    for (auto& p : img.pixels()) {
        p.r = static_cast<std::uint8_t>(std::clamp(p.r + delta, 0, 255));
        p.g = static_cast<std::uint8_t>(std::clamp(p.g + delta, 0, 255));
        p.b = static_cast<std::uint8_t>(std::clamp(p.b + delta, 0, 255));
    }

    return img;
}
