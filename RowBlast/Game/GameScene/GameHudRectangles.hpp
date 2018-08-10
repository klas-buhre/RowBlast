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

    private:
        void CreatePiecesRectangle(Pht::IEngine& engine, const CommonResources& commonResources);
        void FillStencilBuffer(Pht::OfflineRasterizer& rasterizer,
                               const Pht::Vec2& size,
                               float cornerRadius,
                               float padding);
        void DrawPiecesRectangleBorder(Pht::OfflineRasterizer& rasterizer, const Pht::Vec2& size);
        void DrawPiecesRectangleMainArea(Pht::OfflineRasterizer& rasterizer, const Pht::Vec2& size);
        
        std::unique_ptr<Pht::RenderableObject> mPiecesRectangleRenderable;
    };
}

#endif
