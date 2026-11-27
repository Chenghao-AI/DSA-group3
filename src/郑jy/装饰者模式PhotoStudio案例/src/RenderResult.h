#pragma once

#include "Photo.h"

#include <filesystem>

//记录两处便于复核装饰顺序的像素。
struct RenderResult {
    Pixel center;
    Pixel bottomRight;
};

RenderResult renderToPng(const Photo& photo, const std::filesystem::path& filename);
