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

        Pht::RenderableObject& GetPauseButtonRectangle() const {
            return *mPauseButtonRectangle;
        }

        Pht::RenderableObject& GetPressedPauseButtonRectangle() const {
            return *mPressedPauseButtonRectangle;
        }

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
        CreateRectangle(Pht::IEngine& engine,
                        const CommonResources& commonResources,
                        const Pht::Vec2& size,
                        float tilt,
                        const Pht::Vec4& colorSubtract,
                        bool drawDividerLine);
        void FillStencilBuffer(Pht::OfflineRasterizer& rasterizer,
                               const Pht::Vec2& size,
                               float cornerRadius,
                               float padding);
        void DrawRectangleBorder(Pht::OfflineRasterizer& rasterizer,
                                 const Pht::Vec2& size,
                                 const Pht::Vec4& colorSubtract);
        void DrawRectangleMainArea(Pht::OfflineRasterizer& rasterizer,
                                   const Pht::Vec2& size,
                                   const Pht::Vec4& colorSubtract);
        void DrawLine(Pht::OfflineRasterizer& rasterizer, const Pht::Vec4& colorSubtract);
        
        std::unique_ptr<Pht::RenderableObject> mPauseButtonRectangle;
        std::unique_ptr<Pht::RenderableObject> mPressedPauseButtonRectangle;
        std::unique_ptr<Pht::RenderableObject> mNextPiecesRectangle;
        std::unique_ptr<Pht::RenderableObject> mSelectablePiecesRectangle;
        std::unique_ptr<Pht::RenderableObject> mPressedSelectablePiecesRectangle;
    };
}

#endif
