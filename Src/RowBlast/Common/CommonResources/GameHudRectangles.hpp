#ifndef GameHudRectangles_hpp
#define GameHudRectangles_hpp

#include <memory>

// Engine includes.
#include "RenderableObject.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class SoftwareRasterizer;
}

namespace RowBlast {
    class CommonResources;
    
    class GameHudRectangles {
    public:
        GameHudRectangles(Pht::IEngine& engine, const CommonResources& commonResources);

        Pht::RenderableObject& GetNextPiecesRectangle() const {
            return *mNextPiecesRectangle;
        }

        Pht::RenderableObject& GetSelectablePiecesRectangle() const {
            return *mSelectablePiecesRectangle;
        }

        Pht::RenderableObject& GetPressedSelectablePiecesRectangle() const {
            return *mPressedSelectablePiecesRectangle;
        }

    private:
        std::unique_ptr<Pht::RenderableObject>
        CreateNextRectangle(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            const Pht::Vec2& size);
        std::unique_ptr<Pht::RenderableObject>
        CreateSelectablesRectangle(Pht::IEngine& engine,
                                   const CommonResources& commonResources,
                                   const Pht::Vec2& size,
                                   float tilt,
                                   const Pht::Vec4& colorSubtract);
        void DrawNextRectangleUpperBarGradientArea(Pht::SoftwareRasterizer& rasterizer,
                                                   const Pht::Vec2& size);
        void DrawNextRectangleUpperBarGlossyArea(Pht::SoftwareRasterizer& rasterizer,
                                                 const Pht::Vec2& size);
        void DrawSelectablesRectangleBorder(Pht::SoftwareRasterizer& rasterizer,
                                            const Pht::Vec2& size,
                                            const Pht::Vec4& colorSubtract);
        void DrawSelectablesRectangleCheckedArea(Pht::SoftwareRasterizer& rasterizer,
                                                 const Pht::Vec2& size,
                                                 const Pht::Vec4& colorSubtract);
        void DrawNextRectangleMainArea(Pht::SoftwareRasterizer& rasterizer, const Pht::Vec2& size);
        void DrawSelectablesRectangleMainArea(Pht::SoftwareRasterizer& rasterizer,
                                              const Pht::Vec2& size,
                                              const Pht::Vec4& colorSubtract);
        void DrawActiveSlotArea(Pht::SoftwareRasterizer& rasterizer,
                                const Pht::Vec2& size,
                                const Pht::Vec4& colorSubtract);
        void DrawGrid(Pht::SoftwareRasterizer& rasterizer,
                      const Pht::Vec2& rectangleSize,
                      const Pht::Vec2& position,
                      const Pht::Vec2& gridSize,
                      const Pht::Vec4& colorSubtract);
        void DrawLine(Pht::SoftwareRasterizer& rasterizer, const Pht::Vec4& colorSubtract);
        
        std::unique_ptr<Pht::RenderableObject> mNextPiecesRectangle;
        std::unique_ptr<Pht::RenderableObject> mSelectablePiecesRectangle;
        std::unique_ptr<Pht::RenderableObject> mPressedSelectablePiecesRectangle;
    };
}

#endif
