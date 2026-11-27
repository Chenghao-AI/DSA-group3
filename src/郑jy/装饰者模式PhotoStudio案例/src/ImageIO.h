#pragma once

#include "Image.h"

#include <filesystem>

//把照片文件转换成统一的 RGB 矩阵，装饰器无需知道文件格式。
Image loadImage(const std::filesystem::path& filename);
void savePng(const std::filesystem::path& filename, const Image& img);
