#include "Blur.h"
#include "Brightness.h"
#include "Frame.h"
#include "Grayscale.h"
#include "LandscapePhoto.h"
#include "PhotoStudioDemo.h"
#include "PortraitPhoto.h"
#include "RenderResult.h"
#include "Watermark.h"

#include <cwctype>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

namespace fs = std::filesystem;

void printUsage() {
    std::cout << "PhotoStudio --demo [photo-directory [result-directory]]\n"
              << "PhotoStudio [--steps] portrait|landscape input.jpg|input.png output.png [effect ...]\n"
              << "Demo defaults: pictures/photos and pictures/results, relative to the working directory.\n"
              << "Effects in processing order: grayscale, watermark, frame, brightness[=delta], "
                 "blur[=radius]\n"
              << "Brightness: -255..255, default +60. Blur: pixels, 0 or omitted for automatic.\n"
              << "--steps: write the source and each intermediate PNG alongside the result.\n";
}

int readNumber(std::wstring_view text) {
    std::size_t parsed = 0;
    const int value = std::stoi(std::wstring(text), &parsed);
    if (parsed != text.size()) {
        throw std::invalid_argument("Expected an integer effect parameter.");
    }
    return value;
}

//只负责按用户输入组装对象，图像处理仍由各装饰器实现。
PhotoPtr addEffect(const PhotoPtr& photo, std::wstring_view spec, std::string& expression) {
    const auto equals = spec.find(L'=');
    const auto name = spec.substr(0, equals);
    const bool hasArgument = equals != std::wstring_view::npos;
    if (hasArgument && name != L"brightness" && name != L"blur") {
        throw std::invalid_argument("Only brightness and blur take a numeric parameter.");
    }
    const int argument = hasArgument ? readNumber(spec.substr(equals + 1)) : 0;

    PhotoPtr next;
    std::string decorator;
    if (name == L"grayscale") {
        decorator = "Grayscale";
        next = std::make_shared<Grayscale>(photo);
    } else if (name == L"watermark") {
        decorator = "Watermark";
        next = std::make_shared<Watermark>(photo);
    } else if (name == L"frame") {
        decorator = "Frame";
        next = std::make_shared<Frame>(photo);
    } else if (name == L"brightness") {
        decorator = "Brightness";
        next = hasArgument ? std::make_shared<Brightness>(photo, argument)
                           : std::make_shared<Brightness>(photo);
    } else if (name == L"blur") {
        decorator = "Blur";
        next = std::make_shared<Blur>(photo, argument);
    } else {
        throw std::invalid_argument("Unknown effect. Use --help to see the available effects.");
    }

    expression = "std::make_shared<" + decorator + ">(" + expression
        + (hasArgument ? ", " + std::to_string(argument) : "") + ")";
    return next;
}

void checkOutputPath(const fs::path& input, const fs::path& output) {
    if (fs::exists(output) && fs::equivalent(input, output)) {
        throw std::invalid_argument("The output path refers to the input image.");
    }
}

void editPhoto(int argc, wchar_t* argv[]) {
    const bool saveSteps = std::wstring_view(argv[1]) == L"--steps";
    const int first = saveSteps ? 2 : 1;
    if (argc < first + 3) {
        throw std::invalid_argument("Specify the photo type, input and output. See --help.");
    }

    const std::wstring_view kind(argv[first]);
    const fs::path input(argv[first + 1]);
    const fs::path output(argv[first + 2]);
    auto extension = output.extension().wstring();
    for (auto& character : extension) {
        character = static_cast<wchar_t>(std::towlower(character));
    }
    if (extension != L".png") {
        throw std::invalid_argument("Use a .png extension for the output.");
    }

    PhotoPtr photo;
    if (kind == L"portrait") {
        photo = std::make_shared<PortraitPhoto>(input);
    } else if (kind == L"landscape") {
        photo = std::make_shared<LandscapePhoto>(input);
    } else {
        throw std::invalid_argument("The photo type must be portrait or landscape.");
    }

    std::string expression = "original";
    std::vector<PhotoPtr> steps{photo};
    for (int i = first + 3; i < argc; ++i) {
        photo = addEffect(photo, argv[i], expression);
        steps.push_back(photo);
    }

    checkOutputPath(input, output);
    if (saveSteps) {
        const auto directory = output.parent_path() / (output.stem().wstring() + L"-steps");
        for (std::size_t i = 0; i < steps.size(); ++i) {
            const auto step = directory / ("step-" + std::to_string(i) + ".png");
            checkOutputPath(input, step);
            renderToPng(*steps[i], step);
        }
    }

    //处理顺序来自参数，虚函数调用则从最后一层装饰器开始。
    std::cout << "C++ nesting: " << expression << '\n';
    renderToPng(*photo, output);
}

} //命名空间

int wmain(int argc, wchar_t* argv[]) {
    try {
        if (argc == 1 || std::wstring_view(argv[1]) == L"--help") {
            printUsage();
            return 0;
        }
        if (std::wstring_view(argv[1]) == L"--demo") {
            if (argc > 4) {
                throw std::invalid_argument("--demo takes at most two directory arguments.");
            }
            const fs::path photos = argc >= 3 ? argv[2] : L"pictures/photos";
            const fs::path results = argc >= 4 ? argv[3] : L"pictures/results";
            runDemo(photos, results);
        } else {
            editPhoto(argc, argv);
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "PhotoStudio: " << error.what() << '\n';
        return 1;
    }
}
