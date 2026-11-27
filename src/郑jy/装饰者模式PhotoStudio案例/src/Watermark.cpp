#include "Watermark.h"

#include <algorithm>
#include <array>
#include <string_view>
#include <utility>

namespace {

constexpr Pixel red{255, 0, 0};

//把“2026”的点阵排成七行，每个 1 对应一个红色方块。
constexpr std::array<std::string_view, 7> mark{
    "01110 01110 01110 01110",
    "10001 10001 10001 10000",
    "00001 10011 00001 10000",
    "00110 10101 00110 11110",
    "01000 11001 01000 10001",
    "10000 10001 10000 10001",
    "11111 01110 11111 01110",
};

void drawCenterMark(Image& img, int shorterSide) {
    const int scale = std::max(1, shorterSide / 115);
    //中心采样点落在第三个数字的红色笔画上。
    const int left = img.width() / 2 - 14 * scale - scale / 2;
    const int top = img.height() / 2 - 3 * scale - scale / 2;

    const int columns = static_cast<int>(mark.front().size());
    const int rows = static_cast<int>(mark.size());
    if (left < 0 || top < 0 ||
        left + columns * scale > img.width() || top + rows * scale > img.height()) {
        //小图像沿用原案例的单像素中心标记。
        img(img.width() / 2, img.height() / 2) = red;
        return;
    }

    for (int y = 0; y < rows * scale; ++y) {
        for (int x = 0; x < columns * scale; ++x) {
            if (mark[y / scale][x / scale] == '1') {
                img(left + x, top + y) = red;
            }
        }
    }
}

} //命名空间

Watermark::Watermark(PhotoPtr wrapped)
    : EffectDecorator(std::move(wrapped)) {}

std::string Watermark::getDescription() const {
    return photo->getDescription() + ", Watermark";
}

Image Watermark::render() const {
    Image img = photo->render();

    const int shorterSide = std::min(img.width(), img.height());
    drawCenterMark(img, shorterSide);

    //角标紧贴右下角，落在 Frame 的相框范围内。
    const int tagHeight = std::max(1, shorterSide / 24);
    const int tagWidth = std::min(img.width(), 3 * (tagHeight - 1) + 1);
    for (int y = img.height() - tagHeight; y < img.height(); ++y) {
        for (int x = img.width() - tagWidth; x < img.width(); ++x) {
            img(x, y) = red;
        }
    }

    return img;
}
