#include "GameHudArrow.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ISceneManager.hpp"
#include "SoftwareRasterizer.hpp"
#include "IImage.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec4 arrowColor {1.0f, 1.0f, 1.0f, 1.0f};
    const Pht::Vec4 shadowColor {0.27f, 0.27f, 0.27f, 0.5f};
    const Pht::Vec2 arrowRenderableSize {0.6f, 0.6f};
    const Pht::Vec2 shadowOffset {0.05f, 0.05f};
    const Pht::Vec2 arrowLineSize {0.13f, 0.3f};
    constexpr auto arrowTriangleHeight = 0.195f;
    
    std::unique_ptr<Pht::SoftwareRasterizer> CreateRasterizer(Pht::IEngine& engine,
                                                              const CommonResources& commonResources,
                                                              const Pht::Vec2& size) {
        auto& renderer = engine.GetRenderer();
        auto& renderBufferSize = renderer.GetRenderBufferSize();
        auto& frustumSize = commonResources.GetHudFrustumSizePotentiallyZoomedScreen();
        
        auto xScaleFactor =
            static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x);

        auto yScaleFactor =
            static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y);
        
        Pht::IVec2 imageSize {
            static_cast<int>(size.x * xScaleFactor) * 2,
            static_cast<int>(size.y * yScaleFactor) * 2
        };
    
        return std::make_unique<Pht::SoftwareRasterizer>(size, imageSize);
    }
    
    void DrawArrow(Pht::SoftwareRasterizer& rasterizer,
                   const Pht::Vec2& position,
                   const Pht::Vec4& color) {
        Pht::Vec2 lineLowerLeft {
            position.x - arrowLineSize.x / 2.0f,
            position.y - arrowLineSize.y / 2.0f
        };
        Pht::Vec2 lineUpperRight {
            position.x + arrowLineSize.x / 2.0f,
            position.y + arrowLineSize.y / 2.0f
        };
        rasterizer.DrawRectangle(lineUpperRight, lineLowerLeft, color, Pht::DrawOver::Yes);
        
        Pht::Vec2 leftTriangleUpperLeft {
            position.x - arrowTriangleHeight,
            position.y - arrowLineSize.y / 2.0f
        };
        Pht::Vec2 leftTriangleLowerRight {
            position.x,
            position.y - arrowLineSize.y / 2.0f - arrowTriangleHeight
        };
        rasterizer.DrawTiltedTrapezoid225(leftTriangleUpperLeft,
                                          leftTriangleLowerRight,
                                          arrowTriangleHeight,
                                          color,
                                          Pht::DrawOver::Yes);

        Pht::Vec2 rightTriangleUpperRight {
            position.x + arrowTriangleHeight,
            position.y - arrowLineSize.y / 2.0f
        };
        Pht::Vec2 rightTriangleLowerLeft {
            position.x,
            position.y - arrowLineSize.y / 2.0f - arrowTriangleHeight
        };
        rasterizer.DrawTiltedTrapezoid315(rightTriangleUpperRight,
                                          rightTriangleLowerLeft,
                                          arrowTriangleHeight,
                                          color,
                                          Pht::DrawOver::Yes);
    }
}

GameHudArrow::GameHudArrow(Pht::IEngine& engine, const CommonResources& commonResources) {
    auto rasterizer = CreateRasterizer(engine, commonResources, arrowRenderableSize);
    
    DrawArrow(*rasterizer,
              arrowRenderableSize / 2.0f - shadowOffset + Pht::Vec2{0.0f, arrowTriangleHeight / 2.0f},
              shadowColor);
    DrawArrow(*rasterizer,
              arrowRenderableSize / 2.0f + Pht::Vec2{0.0f, arrowTriangleHeight / 2.0f},
              arrowColor);
    
    auto image = rasterizer->ProduceImage();
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    Pht::QuadMesh quadMesh {arrowRenderableSize.x, arrowRenderableSize.y};
    mArrowRenderable = engine.GetSceneManager().CreateRenderableObject(quadMesh, imageMaterial);
}
