#pragma once

#include "Photo.h"

#include <filesystem>

//具体组件，代表一张风景原图。
class LandscapePhoto : public Photo {
public:
    explicit LandscapePhoto(const std::filesystem::path& filename);
    Image render() const override;

private:
    Image original;
};
