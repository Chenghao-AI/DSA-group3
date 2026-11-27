#pragma once

#include <filesystem>

//运行三张照片的完整装饰链，并验证换序实验。
void runDemo(const std::filesystem::path& photos, const std::filesystem::path& results);
