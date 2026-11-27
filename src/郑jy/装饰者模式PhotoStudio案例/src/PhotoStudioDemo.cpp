#include "PhotoStudioDemo.h"

#include "Blur.h"
#include "Brightness.h"
#include "Frame.h"
#include "Grayscale.h"
#include "LandscapePhoto.h"
#include "PortraitPhoto.h"
#include "RenderResult.h"
#include "Watermark.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {

namespace fs = std::filesystem;

constexpr Pixel red{255, 0, 0};
constexpr Pixel grayRed{76, 76, 76};
constexpr Pixel green{0, 180, 0};
constexpr Pixel lightRed{255, 60, 60};

void verify(bool passed, const char* experiment) {
    if (!passed) {
        throw std::runtime_error(std::string("Pixel verification failed: ") + experiment);
    }
    std::cout << "  PASS: " << experiment << "\n\n";
}

void saveAllEffects(PhotoPtr photo, const fs::path& output) {
    //像 StarbuzzCoffee 加配料一样，使用同一个变量逐层包裹。
    photo = std::make_shared<Grayscale>(photo);
    photo = std::make_shared<Watermark>(photo);
    photo = std::make_shared<Frame>(photo);
    photo = std::make_shared<Brightness>(photo);
    photo = std::make_shared<Blur>(photo);
    renderToPng(*photo, output);
}

} //命名空间

void runDemo(const std::filesystem::path& photos, const std::filesystem::path& results) {
    const PhotoPtr banff = std::make_shared<LandscapePhoto>(photos / "landscape-banff.jpg");
    const PhotoPtr portrait = std::make_shared<PortraitPhoto>(photos / "portrait-collins.png");
    const PhotoPtr city = std::make_shared<LandscapePhoto>(photos / "landscape-city.jpg");

    std::cout << "PhotoStudio | Original -> Grayscale -> Watermark -> Frame -> Brightness -> Blur\n";
    const auto fullProcess = results / "full-process";
    const auto steps = fullProcess / "banff-steps";
    PhotoPtr photo = banff;
    renderToPng(*photo, steps / "00-original.png");
    photo = std::make_shared<Grayscale>(photo);
    renderToPng(*photo, steps / "01-grayscale.png");
    photo = std::make_shared<Watermark>(photo);
    renderToPng(*photo, steps / "02-watermark.png");
    photo = std::make_shared<Frame>(photo);
    renderToPng(*photo, steps / "03-frame.png");
    photo = std::make_shared<Brightness>(photo);
    renderToPng(*photo, steps / "04-brightness.png");
    photo = std::make_shared<Blur>(photo);
    renderToPng(*photo, steps / "05-blur.png");
    saveAllEffects(portrait, fullProcess / "portrait-full.png");
    saveAllEffects(city, fullProcess / "city-full.png");

    const auto orderExperiments = results / "order-experiments";
    std::cout << "\n01 | Grayscale and Watermark\n";
    const auto grayscaleWatermark = orderExperiments / "01-grayscale-watermark";
    PhotoPtr first = std::make_shared<Grayscale>(banff);
    first = std::make_shared<Watermark>(first);
    PhotoPtr second = std::make_shared<Watermark>(banff);
    second = std::make_shared<Grayscale>(second);
    auto a = renderToPng(*first, grayscaleWatermark / "gray-then-watermark.png");
    auto b = renderToPng(*second, grayscaleWatermark / "watermark-then-gray.png");
    verify(a.center == red && b.center == grayRed, "red/gray center watermark");

    std::cout << "02 | Watermark and Frame\n";
    const auto watermarkFrame = orderExperiments / "02-watermark-frame";
    first = std::make_shared<Watermark>(banff);
    first = std::make_shared<Frame>(first);
    second = std::make_shared<Frame>(banff);
    second = std::make_shared<Watermark>(second);
    a = renderToPng(*first, watermarkFrame / "watermark-then-frame.png");
    b = renderToPng(*second, watermarkFrame / "frame-then-watermark.png");
    verify(a.bottomRight == green && b.bottomRight == red, "green/red bottom-right pixel");

    std::cout << "03 | Brightness and Watermark\n";
    const auto brightnessWatermark = orderExperiments / "03-brightness-watermark";
    first = std::make_shared<Brightness>(portrait);
    first = std::make_shared<Watermark>(first);
    second = std::make_shared<Watermark>(portrait);
    second = std::make_shared<Brightness>(second);
    a = renderToPng(*first, brightnessWatermark / "brightness-then-watermark.png");
    b = renderToPng(*second, brightnessWatermark / "watermark-then-brightness.png");
    verify(a.center == red && b.center == lightRed, "red/light red center watermark");

    std::cout << "04 | Blur and Watermark\n";
    const auto blurWatermark = orderExperiments / "04-blur-watermark";
    first = std::make_shared<Blur>(city);
    first = std::make_shared<Watermark>(first);
    second = std::make_shared<Watermark>(city);
    second = std::make_shared<Blur>(second);
    a = renderToPng(*first, blurWatermark / "blur-then-watermark.png");
    b = renderToPng(*second, blurWatermark / "watermark-then-blur.png");
    verify(a.center == red && !(b.center == red), "sharp/mixed center watermark pixel");

    std::cout << "05 | Repeating Brightness\n";
    const auto brightnessRepeat = results / "brightness-repeat";
    first = std::make_shared<Brightness>(portrait);
    second = std::make_shared<Brightness>(first);
    a = renderToPng(*first, brightnessRepeat / "brightness-once.png");
    b = renderToPng(*second, brightnessRepeat / "brightness-twice.png");
    const auto plus60 = [](std::uint8_t channel) {
        return static_cast<std::uint8_t>(std::min(channel + 60, 255));
    };
    const Pixel expected{plus60(a.center.r), plus60(a.center.g), plus60(a.center.b)};
    verify(b.center == expected, "two independent brightness layers");

    std::cout << "Demo complete: 18 PNG files written.\n";
}
