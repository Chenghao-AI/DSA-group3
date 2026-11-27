#pragma once

#include "Photo.h"

#include <filesystem>

//具体组件，代表一张人像原图。
class PortraitPhoto : public Photo {
public:
    explicit PortraitPhoto(const std::filesystem::path& filename);
    Image render() const override;

private:
    Image original;
};
