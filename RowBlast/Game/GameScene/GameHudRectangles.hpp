#ifndef GameHudRectangles_hpp
#define GameHudRectangles_hpp

#include <memory>

// Engine includes.
#include "RenderableObject.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class OfflineRasterizer;
}

namespace RowBlast {
    class CommonResources;
    
    class GameHudRectangles {
    public:
        GameHudRectangles(Pht::IEngine& engine, const CommonResources& commonResources);

        Pht::RenderableObject& GetPiecesRectangleRenderable() const {
            return *mPiecesRectangleRenderable;
        }

        Pht::RenderableObject& GetPressedPiecesRectangleRenderable() const {
            return *mPressedPiecesRectangleRenderable;
        }

    private:
        std::unique_ptr<Pht::RenderableObject>
        CreatePiecesRectangle(Pht::IEngine& engine,
                              const CommonResources& commonResources,
                              const Pht::Vec4& colorSubtract);
        void FillStencilBuffer(Pht::OfflineRasterizer& rasterizer,
                               const Pht::Vec2& size,
                               float cornerRadius,
                               float padding);
        void DrawPiecesRectangleBorder(Pht::OfflineRasterizer& rasterizer,
                                       const Pht::Vec2& size,
                                       const Pht::Vec4& colorSubtract);
        void DrawPiecesRectangleMainArea(Pht::OfflineRasterizer& rasterizer,
                                         const Pht::Vec2& size,
                                         const Pht::Vec4& colorSubtract);
        
        std::unique_ptr<Pht::RenderableObject> mPiecesRectangleRenderable;
        std::unique_ptr<Pht::RenderableObject> mPressedPiecesRectangleRenderable;
    };
}

#endif
