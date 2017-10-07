#ifndef OfflineRasterizer_hpp
#define OfflineRasterizer_hpp

#include <memory>
#include <vector>

#include "Vector.hpp"

namespace Pht {
    class IImage;
    
    enum class DrawOver {
        Yes,
        No
    };
    
    class OfflineRasterizer {
    public:
        OfflineRasterizer(const Vec2& coordinateSystemSize, const IVec2& imageSize);
        
        void ClearBuffer();
        void DrawRectangle(const Vec2& upperRight,
                           const Vec2& lowerLeft,
                           const Vec4& color,
                           DrawOver drawOver = DrawOver::No);
        void DrawTiltedTrapezoid45(const Vec2& upperRight,
                                   const Vec2& lowerLeft,
                                   float width,
                                   const Vec4& color);
        void DrawTiltedTrapezoid135(const Vec2& upperLeft,
                                    const Vec2& lowerRight,
                                    float width,
                                    const Vec4& color);
        void DrawTiltedTrapezoid225(const Vec2& upperLeft,
                                    const Vec2& lowerRight,
                                    float width,
                                    const Vec4& color);
        void DrawTiltedTrapezoid315(const Vec2& upperRight,
                                    const Vec2& lowerLeft,
                                    float width,
                                    const Vec4& color);
        void DrawCircle(const Vec2& center, float radius, float width, const Vec4& color);
        void FillEnclosedArea(const Vec4& color);
        std::unique_ptr<IImage> ProduceImage() const;
        
    private:
        enum class ScanlineState {
            Outside,
            FirstBorder,
            Inside,
            SecondBorder
        };
        
        IVec2 ToPixelCoordinates(const Vec2& point) const;
        void SetPixel(int x, int y, const Vec4& color, DrawOver drawOver = DrawOver::No);
        const Vec4& GetPixel(int x, int y) const;
        bool ShouldSkipLine(int y) const;
        ScanlineState UpdateScanlineFsm(int x, int y, ScanlineState state) const;
        
        Vec2 mCoordSystemSize;
        IVec2 mImageSize;
        std::vector<Vec4> mBuffer;
    };
}

#endif
