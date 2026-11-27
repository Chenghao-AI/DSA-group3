#include "Frame.h"

#include <algorithm>
#include <utility>

Frame::Frame(PhotoPtr wrapped)
    : EffectDecorator(std::move(wrapped)) {}

std::string Frame::getDescription() const {
    return photo->getDescription() + ", Frame";
}

Image Frame::render() const {
    Image img = photo->render();

    //相框宽度随照片短边调整，至少覆盖一圈像素。
    const int thickness = std::max(1, std::min(img.width(), img.height()) / 18);
    const Pixel green{0, 180, 0};

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            if (x < thickness || x >= img.width() - thickness ||
                y < thickness || y >= img.height() - thickness) {
                img(x, y) = green;
            }
        }
    }

    return img;
}
