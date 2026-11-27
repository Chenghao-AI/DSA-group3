#include "ImageIO.h"

#include <windows.h>
#include <wincodec.h>
#include <wrl/client.h>

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {

using Microsoft::WRL::ComPtr;

void check(HRESULT result, const char* action) {
    if (FAILED(result)) {
        std::ostringstream message;
        message << action << " (HRESULT 0x" << std::hex
                << static_cast<std::uint32_t>(result) << ')';
        throw std::runtime_error(message.str());
    }
}

//WIC 是 Windows 自带的组件，使用 COM 管理解码器和编码器。
class ComSession {
public:
    ComSession() {
        check(CoInitializeEx(nullptr, COINIT_MULTITHREADED), "Initialize COM");
    }

    ~ComSession() { CoUninitialize(); }

    ComSession(const ComSession&) = delete;
    ComSession& operator=(const ComSession&) = delete;
};

ComPtr<IWICImagingFactory> makeFactory() {
    ComPtr<IWICImagingFactory> factory;
    check(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                           IID_PPV_ARGS(factory.GetAddressOf())),
          "Create WIC factory");
    return factory;
}

UINT wicByteCount(std::uint64_t pixels) {
    if (pixels > std::numeric_limits<UINT>::max() / sizeof(Pixel)) {
        throw std::length_error("Image is too large for the WIC RGB buffer.");
    }
    return static_cast<UINT>(pixels * sizeof(Pixel));
}

WICBitmapTransformOptions readJpegOrientation(IWICBitmapFrameDecode* frame) {
    ComPtr<IWICMetadataQueryReader> metadata;
    if (FAILED(frame->GetMetadataQueryReader(metadata.GetAddressOf()))) {
        return WICBitmapTransformRotate0;
    }

    PROPVARIANT value{};
    const HRESULT result = metadata->GetMetadataByName(L"/app1/ifd/{ushort=274}", &value);
    const auto orientation = SUCCEEDED(result) && value.vt == VT_UI2 ? value.uiVal : 1;
    PropVariantClear(&value);

    //把 JPEG 的 EXIF 方向落实到像素，方便处理手机的竖拍照片。
    switch (orientation) {
        case 2: return WICBitmapTransformFlipHorizontal;
        case 3: return WICBitmapTransformRotate180;
        case 4: return WICBitmapTransformFlipVertical;
        case 5:
            //WIC 的组合变换先翻转再旋转，分别对应两条对角线。
            return static_cast<WICBitmapTransformOptions>(
                WICBitmapTransformRotate270 | WICBitmapTransformFlipHorizontal);
        case 6: return WICBitmapTransformRotate90;
        case 7:
            return static_cast<WICBitmapTransformOptions>(
                WICBitmapTransformRotate90 | WICBitmapTransformFlipHorizontal);
        case 8: return WICBitmapTransformRotate270;
        default: return WICBitmapTransformRotate0;
    }
}

} //命名空间

Image loadImage(const std::filesystem::path& filename) {
    const ComSession session;
    const auto factory = makeFactory();

    ComPtr<IWICBitmapDecoder> decoder;
    check(factory->CreateDecoderFromFilename(filename.c_str(), nullptr, GENERIC_READ,
                                              WICDecodeMetadataCacheOnDemand,
                                              decoder.GetAddressOf()),
          "Open input image");
    ComPtr<IWICBitmapFrameDecode> frame;
    check(decoder->GetFrame(0, frame.GetAddressOf()), "Decode first frame");

    IWICBitmapSource* source = frame.Get();
    ComPtr<IWICBitmapFlipRotator> rotated;
    GUID container{};
    check(decoder->GetContainerFormat(&container), "Read image format");
    if (IsEqualGUID(container, GUID_ContainerFormatJpeg)) {
        const auto orientation = readJpegOrientation(frame.Get());
        if (orientation != WICBitmapTransformRotate0) {
            check(factory->CreateBitmapFlipRotator(rotated.GetAddressOf()), "Create rotator");
            check(rotated->Initialize(source, orientation), "Apply EXIF orientation");
            source = rotated.Get();
        }
    }

    ComPtr<IWICFormatConverter> rgb;
    check(factory->CreateFormatConverter(rgb.GetAddressOf()), "Create RGB converter");
    check(rgb->Initialize(source, GUID_WICPixelFormat24bppRGB, WICBitmapDitherTypeNone,
                          nullptr, 0.0, WICBitmapPaletteTypeCustom),
          "Convert input image to 24-bit RGB");

    UINT width = 0;
    UINT height = 0;
    check(rgb->GetSize(&width, &height), "Read image size");
    const auto maxDimension = static_cast<UINT>(std::numeric_limits<int>::max());
    if (width > maxDimension || height > maxDimension) {
        throw std::length_error("Image dimensions exceed the supported range.");
    }

    const UINT stride = wicByteCount(width);
    const UINT bytes = wicByteCount(static_cast<std::uint64_t>(width) * height);
    Image img(static_cast<int>(width), static_cast<int>(height));
    check(rgb->CopyPixels(nullptr, stride, bytes, reinterpret_cast<BYTE*>(img.pixels().data())),
          "Read RGB pixels");
    return img;
}

void savePng(const std::filesystem::path& filename, const Image& img) {
    const ComSession session;
    const auto factory = makeFactory();
    const UINT stride = wicByteCount(img.width());
    const UINT bytes = wicByteCount(img.pixels().size());

    if (!filename.parent_path().empty()) {
        std::filesystem::create_directories(filename.parent_path());
    }

    ComPtr<IWICStream> output;
    check(factory->CreateStream(output.GetAddressOf()), "Create PNG stream");
    check(output->InitializeFromFilename(filename.c_str(), GENERIC_WRITE), "Open PNG output");

    ComPtr<IWICBitmapEncoder> encoder;
    check(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, encoder.GetAddressOf()),
          "Create PNG encoder");
    check(encoder->Initialize(output.Get(), WICBitmapEncoderNoCache), "Initialize PNG encoder");

    ComPtr<IWICBitmapFrameEncode> frame;
    check(encoder->CreateNewFrame(frame.GetAddressOf(), nullptr), "Create PNG frame");
    check(frame->Initialize(nullptr), "Initialize PNG frame");
    check(frame->SetSize(img.width(), img.height()), "Set PNG size");

    WICPixelFormatGUID format = GUID_WICPixelFormat24bppBGR;
    check(frame->SetPixelFormat(&format), "Set PNG pixel format");
    if (!IsEqualGUID(format, GUID_WICPixelFormat24bppBGR)) {
        throw std::runtime_error("The PNG encoder did not accept 24-bit BGR.");
    }

    //WIC 的 PNG 编码器接收 BGR，装饰器仍按 RGB 读写图像。
    auto bgr = img.pixels();
    for (auto& p : bgr) {
        std::swap(p.r, p.b);
    }
    check(frame->WritePixels(img.height(), stride, bytes, reinterpret_cast<BYTE*>(bgr.data())),
          "Write PNG pixels");
    check(frame->Commit(), "Finish PNG frame");
    check(encoder->Commit(), "Finish PNG file");
}
