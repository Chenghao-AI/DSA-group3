cmake_minimum_required(VERSION 3.20)
project(PhotoStudio LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "PhotoStudio uses Windows Imaging Component to read and write pictures.")
endif()

add_executable(PhotoStudio
    src/PhotoStudio.cpp
    src/PhotoStudioDemo.cpp
    src/RenderResult.cpp
    src/Image.cpp
    src/ImageIO.cpp
    src/Photo.cpp
    src/PortraitPhoto.cpp
    src/LandscapePhoto.cpp
    src/EffectDecorator.cpp
    src/Grayscale.cpp
    src/Watermark.cpp
    src/Frame.cpp
    src/Brightness.cpp
    src/Blur.cpp
)

set_target_properties(PhotoStudio PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_compile_definitions(PhotoStudio PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(PhotoStudio PRIVATE windowscodecs ole32 uuid)

if(MSVC)
    target_compile_options(PhotoStudio PRIVATE /W4 /utf-8 /permissive-)
elseif(MINGW)
    target_compile_options(PhotoStudio PRIVATE -Wall -Wextra -Wpedantic)
    target_link_options(PhotoStudio PRIVATE -municode)
endif()
