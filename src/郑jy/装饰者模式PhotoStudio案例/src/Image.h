#pragma once

#include "Pixel.h"

#include <vector>

//用连续内存保存任意宽高的 RGB 像素矩阵。
class Image {
public:
    Image(int width, int height);

    int width() const { return width_; }
    int height() const { return height_; }

    //坐标从左上角开始，x 表示列，y 表示行。
    Pixel& operator()(int x, int y);
    const Pixel& operator()(int x, int y) const;

    std::vector<Pixel>& pixels() { return pixels_; }
    const std::vector<Pixel>& pixels() const { return pixels_; }

private:
    int width_;
    int height_;
    std::vector<Pixel> pixels_;
};
