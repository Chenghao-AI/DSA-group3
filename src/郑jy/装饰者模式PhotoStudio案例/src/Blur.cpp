#include "Blur.h"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

//存储三个通道的和，等完整窗口计算完再取整。
struct RgbSum {
    std::int64_t r = 0;
    std::int64_t g = 0;
    std::int64_t b = 0;

    RgbSum() = default;
    explicit RgbSum(Pixel p) : r(p.r), g(p.g), b(p.b) {}

    RgbSum& operator+=(const RgbSum& other) {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    RgbSum& operator-=(const RgbSum& other) {
        r -= other.r;
        g -= other.g;
        b -= other.b;
        return *this;
    }

    Pixel average(std::int64_t count) const {
        return {static_cast<std::uint8_t>(r / count),
                static_cast<std::uint8_t>(g / count),
                static_cast<std::uint8_t>(b / count)};
    }
};

int validWindowSize(int position, int radius, int length) {
    return 1 + std::min(position, radius) + std::min(length - 1 - position, radius);
}

Image boxBlur(const Image& img, int radius) {
    const int width = img.width();
    const int height = img.height();
    Image result(width, height);
    std::vector<RgbSum> columns(width);

    //先累加第一行所能看到的有效邻域行，得到每一列的和。
    for (int y = 0; y <= std::min(radius, height - 1); ++y) {
        for (int x = 0; x < width; ++x) {
            columns[x] += RgbSum(img(x, y));
        }
    }

    for (int y = 0; y < height; ++y) {
        RgbSum window;
        for (int x = 0; x <= std::min(radius, width - 1); ++x) {
            window += columns[x];
        }
        const std::int64_t rows = validWindowSize(y, radius, height);

        //横向滑动窗口，只减离开的列、加新进入的列。
        for (int x = 0; x < width; ++x) {
            result(x, y) = window.average(rows * validWindowSize(x, radius, width));
            if (x >= radius) {
                window -= columns[x - radius];
            }
            if (x < width - 1 - radius) {
                window += columns[x + radius + 1];
            }
        }

        //换到下一行时，同样更新竖直窗口，始终只读取输入图像。
        for (int x = 0; x < width; ++x) {
            if (y >= radius) {
                columns[x] -= RgbSum(img(x, y - radius));
            }
            if (y < height - 1 - radius) {
                columns[x] += RgbSum(img(x, y + radius + 1));
            }
        }
    }

    return result;
}

} //命名空间

Blur::Blur(PhotoPtr wrapped, int pixels)
    : EffectDecorator(std::move(wrapped)), radius(pixels) {
    if (radius < 0) {
        throw std::invalid_argument("Blur radius must be nonnegative.");
    }
}

std::string Blur::getDescription() const {
    return photo->getDescription() + ", Blur";
}

Image Blur::render() const {
    Image img = photo->render();

    //5×5 小图的默认半径为 1，实拍照片随短边按比例增大。
    const int chosenRadius = radius == 0
        ? std::max(1, std::min(img.width(), img.height()) / 100)
        : radius;
    const int effectiveRadius =
        std::min(chosenRadius, std::max(img.width(), img.height()) - 1);

    return boxBlur(img, effectiveRadius);
}
