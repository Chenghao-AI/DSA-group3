#include "RenderResult.h"

#include "ImageIO.h"

#include <iostream>

namespace {

void printRgb(Pixel pixel) {
    std::cout << '(' << static_cast<int>(pixel.r) << ", "
              << static_cast<int>(pixel.g) << ", "
              << static_cast<int>(pixel.b) << ')';
}

} //命名空间

RenderResult renderToPng(const Photo& photo, const std::filesystem::path& filename) {
    //虚函数会从最外层装饰器开始调用。
    const Image img = photo.render();
    savePng(filename, img);

    const RenderResult result{
        img(img.width() / 2, img.height() / 2),
        img(img.width() - 1, img.height() - 1),
    };
    std::cout << filename.filename().u8string() << "  [" << img.width()
              << " x " << img.height() << "]\n"
              << "  " << photo.getDescription() << "\n  Center RGB: ";
    printRgb(result.center);
    std::cout << "  Bottom-right RGB: ";
    printRgb(result.bottomRight);
    std::cout << '\n';
    return result;
}
