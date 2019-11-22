#ifndef MenuWindow_hpp
#define MenuWindow_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
    class SoftwareRasterizer;
}

namespace RowBlast {
    class CommonResources;
    
    enum class PotentiallyZoomedScreen {
        Yes,
        No
    };
    
    class MenuWindow {
    public:
        enum class Size {
            Large,
            Medium,
            Small,
            Smallest
        };
        
        enum class Style {
            Bright,
            Dark
        };
        
        MenuWindow(Pht::IEngine& engine,
                   const CommonResources& commonResources,
                   Size size,
                   Style style);

        Pht::RenderableObject& GetRenderable() const {
            return *mRenderableObject;
        }
        
        const Pht::Vec2& GetSize() const {
            return mSize;
        }
        
    private:
        void FillStencilBuffer(Pht::SoftwareRasterizer& rasterizer,
                               float cornerRadius,
                               float padding);
        void DrawBrightCaptionBar(Pht::SoftwareRasterizer& rasterizer, float captionBarHeight);
        void DrawBrightMainArea(Pht::SoftwareRasterizer& rasterizer, float captionBarHeight);
        void DrawDarkBorder(Pht::SoftwareRasterizer& rasterizer);
        void DrawBrightBorder(Pht::SoftwareRasterizer& rasterizer);
        void DrawBorder(Pht::SoftwareRasterizer& rasterizer, const Pht::Vec4& color);
        void DrawDarkMainArea(Pht::SoftwareRasterizer& rasterizer);
        
        std::unique_ptr<Pht::RenderableObject> mRenderableObject;
        Pht::Vec2 mSize;
    };
}

#endif
