#ifndef MenuWindow_hpp
#define MenuWindow_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
    class OfflineRasterizer;
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
            Small
        };
        
        MenuWindow(Pht::IEngine& engine,
                   const CommonResources& commonResources,
                   Size size,
                   PotentiallyZoomedScreen potentiallyZoomed);

        Pht::RenderableObject& GetRenderable() const {
            return *mRenderableObject;
        }
        
        const Pht::Vec2& GetSize() const {
            return mSize;
        }
        
    private:
        void FillStencilBuffer(Pht::OfflineRasterizer& rasterizer);
        void DrawCaptionBar(Pht::OfflineRasterizer& rasterizer);
        void DrawMainArea(Pht::OfflineRasterizer& rasterizer);
        
        std::unique_ptr<Pht::RenderableObject> mRenderableObject;
        Pht::Vec2 mSize;
    };
}

#endif
