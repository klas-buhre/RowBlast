#include "OfflineRasterizer.hpp"

#include <assert.h>

#include "IImage.hpp"

using namespace Pht;

namespace {
    Vec4 transparentPixel {0.0f, 0.0f, 0.0f, 0.0f};
    
    unsigned char MapColorToUChar(float color) {
        if (color < 0.0f) {
            return 0;
        }
        
        if (color > 1.0f) {
            return 255;
        }
        
        return static_cast<unsigned char>(color * 255.0f);
    }
    
    class RgbaImage: public IImage {
    public:
        RgbaImage(const IVec2& size, const std::vector<Vec4>& buffer) :
            mSize {size} {
            
            auto numPixels {size.x * size.y};
            assert(numPixels == buffer.size());
        
            mData = std::make_unique<unsigned char[]>(numPixels * 4);
            auto* data {&mData[0]};
            
            for (const auto& pixel: buffer) {
                *data++ = MapColorToUChar(pixel.x);
                *data++ = MapColorToUChar(pixel.y);
                *data++ = MapColorToUChar(pixel.z);
                *data++ = MapColorToUChar(pixel.w);
            }
        }

        ImageFormat GetFormat() const override {
            return ImageFormat::Rgba;
        }
        
        int GetBitsPerComponent() const override {
            return 8;
        }
        
        IVec2 GetSize() const override {
            return mSize;
        }
        
        void* GetImageData() const override {
            return mData.get();
        }
        
        bool HasPremultipliedAlpha() const override {
            return false;
        }
        
    private:
        IVec2 mSize;
        std::unique_ptr<unsigned char[]> mData;
    };
}

OfflineRasterizer::OfflineRasterizer(const Vec2& coordinateSystemSize, const IVec2& imageSize) :
    mCoordSystemSize {coordinateSystemSize},
    mImageSize {imageSize} {
    
    mBuffer.resize(imageSize.x * imageSize.y);
    ClearBuffer();
}

void OfflineRasterizer::ClearBuffer() {
    for (auto& pixel: mBuffer) {
        pixel = transparentPixel;
    }
}

IVec2 OfflineRasterizer::ToPixelCoordinates(const Vec2& point) const {
    auto xScaleFactor {static_cast<float>(mImageSize.x) / mCoordSystemSize.x};
    auto yScaleFactor {static_cast<float>(mImageSize.y) / mCoordSystemSize.y};
    
    return {
        static_cast<int>(point.x * xScaleFactor),
        static_cast<int>(point.y * yScaleFactor)
    };
}

void OfflineRasterizer::SetPixel(int x, int y, const Vec4& color, DrawOver drawOver) {
    if (x < 0 || x >= mImageSize.x || y < 0 || y >= mImageSize.y) {
        return;
    }

    auto offset {(mImageSize.y - y - 1) * mImageSize.x + x};

    switch (drawOver) {
        case DrawOver::Yes:
            mBuffer[offset] = color;
            break;
        case DrawOver::No:
            if (mBuffer[offset] == transparentPixel) {
                mBuffer[offset] = color;
            }
            break;
    }
}

const Vec4& OfflineRasterizer::GetPixel(int x, int y) const {
    auto offset {(mImageSize.y - y - 1) * mImageSize.x + x};
    assert(offset >= 0 && offset < mImageSize.x * mImageSize.y);
    
    return mBuffer[offset];
}

void OfflineRasterizer::DrawRectangle(const Vec2& upperRight,
                                      const Vec2& lowerLeft,
                                      const Vec4& color,
                                      DrawOver drawOver) {
    auto upperRightPixelCoord {ToPixelCoordinates(upperRight)};
    auto lowerLeftPixelCoord {ToPixelCoordinates(lowerLeft)};
    
    for (auto y {lowerLeftPixelCoord.y}; y <= upperRightPixelCoord.y; ++y) {
        for (auto x {lowerLeftPixelCoord.x}; x <= upperRightPixelCoord.x; ++x) {
            SetPixel(x, y, color, drawOver);
        }
    }
}

void OfflineRasterizer::DrawTiltedTrapezoid45(const Vec2& upperRight,
                                              const Vec2& lowerLeft,
                                              float width,
                                              const Vec4& color,
                                              DrawOver drawOver) {
    auto upperRightPixelCoord {ToPixelCoordinates(upperRight)};
    auto lowerLeftPixelCoord {ToPixelCoordinates(lowerLeft)};
    auto yScaleFactor {static_cast<float>(mImageSize.y) / mCoordSystemSize.y};
    auto scanColumnHeight {static_cast<int>(width * yScaleFactor * std::sqrt(2.0f) + 0.5f)};
    auto yColumnStart {lowerLeftPixelCoord.y};

    for (auto x {lowerLeftPixelCoord.x}; x <= upperRightPixelCoord.x; ++x) {
        ++yColumnStart;
        auto yColumnEnd {yColumnStart - scanColumnHeight};
        
        if (yColumnEnd < lowerLeftPixelCoord.y) {
            yColumnEnd = lowerLeftPixelCoord.y;
        }
        
        for (auto y {yColumnStart}; y >= yColumnEnd; --y) {
            SetPixel(x, y, color, drawOver);
        }
    }
}

void OfflineRasterizer::DrawTiltedTrapezoid135(const Vec2& upperLeft,
                                               const Vec2& lowerRight,
                                               float width,
                                               const Vec4& color,
                                               DrawOver drawOver) {
    auto upperLeftPixelCoord {ToPixelCoordinates(upperLeft)};
    auto lowerRightPixelCoord {ToPixelCoordinates(lowerRight)};
    auto yScaleFactor {static_cast<float>(mImageSize.y) / mCoordSystemSize.y};
    auto scanColumnHeight {static_cast<int>(width * yScaleFactor * std::sqrt(2.0f) + 0.5f)};
    auto yColumnStart {upperLeftPixelCoord.y};

    for (auto x {upperLeftPixelCoord.x}; x <= lowerRightPixelCoord.x; ++x) {
        --yColumnStart;
        auto yColumnEnd {yColumnStart - scanColumnHeight};
        
        if (yColumnEnd < lowerRightPixelCoord.y) {
            yColumnEnd = lowerRightPixelCoord.y;
        }
        
        for (auto y {yColumnStart}; y >= yColumnEnd; --y) {
            SetPixel(x, y, color, drawOver);
        }
    }
}

void OfflineRasterizer::DrawTiltedTrapezoid225(const Vec2& upperLeft,
                                               const Vec2& lowerRight,
                                               float width,
                                               const Vec4& color,
                                               DrawOver drawOver) {
    auto upperLeftPixelCoord {ToPixelCoordinates(upperLeft)};
    auto lowerRightPixelCoord {ToPixelCoordinates(lowerRight)};
    auto yScaleFactor {static_cast<float>(mImageSize.y) / mCoordSystemSize.y};
    auto scanColumnHeight {static_cast<int>(width * yScaleFactor * std::sqrt(2.0f) + 0.5f)};
    auto yColumnEnd {upperLeftPixelCoord.y};

    for (auto x {upperLeftPixelCoord.x}; x <= lowerRightPixelCoord.x; ++x) {
        --yColumnEnd;
        auto yColumnStart {yColumnEnd + scanColumnHeight};
        
        if (yColumnStart > upperLeftPixelCoord.y) {
            yColumnStart = upperLeftPixelCoord.y;
        }
        
        for (auto y {yColumnStart}; y >= yColumnEnd; --y) {
            SetPixel(x, y, color, drawOver);
        }
    }
}

void OfflineRasterizer::DrawTiltedTrapezoid315(const Vec2& upperRight,
                                               const Vec2& lowerLeft,
                                               float width,
                                               const Vec4& color,
                                               DrawOver drawOver) {
    auto upperRightPixelCoord {ToPixelCoordinates(upperRight)};
    auto lowerLeftPixelCoord {ToPixelCoordinates(lowerLeft)};
    auto yScaleFactor {static_cast<float>(mImageSize.y) / mCoordSystemSize.y};
    auto scanColumnHeight {static_cast<int>(width * yScaleFactor * std::sqrt(2.0f) + 0.5f)};
    auto yColumnEnd {lowerLeftPixelCoord.y};

    for (auto x {lowerLeftPixelCoord.x}; x <= upperRightPixelCoord.x; ++x) {
        ++yColumnEnd;
        auto yColumnStart {yColumnEnd + scanColumnHeight};
        
        if (yColumnStart > upperRightPixelCoord.y) {
            yColumnStart = upperRightPixelCoord.y;
        }
        
        for (auto y {yColumnStart}; y >= yColumnEnd; --y) {
            SetPixel(x, y, color, drawOver);
        }
    }
}

void OfflineRasterizer::DrawCircle(const Vec2& center,
                                   float radius,
                                   float width,
                                   const Vec4& color) {
    auto centerPixelCoord {ToPixelCoordinates(center)};
    auto scaleFactor {static_cast<float>(mImageSize.y) / mCoordSystemSize.y};
    auto radiusInPixels {radius * scaleFactor};
    auto widthInPixels {width * scaleFactor};
    
    Pht::Vec2 centerPixelCoordFloat {
        static_cast<float>(centerPixelCoord.x) + 0.5f,
        static_cast<float>(centerPixelCoord.y) + 0.5f
    };
    
    for (auto y {0}; y < mImageSize.y; ++y) {
        for (auto x {0}; x < mImageSize.x; ++x) {
            Pht::Vec2 pixelCoordFloat {static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f};
            auto distToCenter {(pixelCoordFloat - centerPixelCoordFloat).Length()};
            
            if (distToCenter <= radiusInPixels && distToCenter >= radiusInPixels - widthInPixels) {
                SetPixel(x, y, color);
            }
        }
    }
}

void OfflineRasterizer::FillEnclosedArea(const Vec4& color) {
    for (auto y {0}; y < mImageSize.y; ++y) {
        if (ShouldSkipLine(y)) {
            continue;
        }
        
        auto state {ScanlineState::Outside};
        
        for (auto x {0}; x < mImageSize.x; ++x) {
            state = UpdateScanlineFsm(x, y, state);
            
            if (state == ScanlineState::Inside) {
                SetPixel(x, y, color);
            }
        }
    }
}

bool OfflineRasterizer::ShouldSkipLine(int y) const {
    auto state {ScanlineState::Outside};

    for (auto x {0}; x < mImageSize.x; ++x) {
        state = UpdateScanlineFsm(x, y, state);
    }
    
    if (state == ScanlineState::Inside) {
        return true;
    }
    
    return false;
}

OfflineRasterizer::ScanlineState OfflineRasterizer::UpdateScanlineFsm(int x,
                                                                      int y,
                                                                      ScanlineState state) const {
    switch (state) {
        case ScanlineState::Outside:
            if (GetPixel(x, y) != transparentPixel) {
                state = ScanlineState::FirstBorder;
            }
            break;
        case ScanlineState::FirstBorder:
            if (GetPixel(x, y) == transparentPixel) {
                state = ScanlineState::Inside;
            }
            break;
        case ScanlineState::Inside:
            if (GetPixel(x, y) != transparentPixel) {
                state = ScanlineState::SecondBorder;
            }
            break;
        case ScanlineState::SecondBorder:
            if (GetPixel(x, y) == transparentPixel) {
                state = ScanlineState::Outside;
            }
            break;
    }
    
    return state;
}

std::unique_ptr<IImage> OfflineRasterizer::ProduceImage() const {
    return std::make_unique<RgbaImage>(mImageSize, mBuffer);
}
