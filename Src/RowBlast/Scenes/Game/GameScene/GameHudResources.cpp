#include "GameHudResources.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ISceneManager.hpp"
#include "SoftwareRasterizer.hpp"
#include "IImage.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec4 downArrowColor {1.0f, 1.0f, 1.0f, 1.0f};
    const Pht::Vec4 shadowColor {0.27f, 0.27f, 0.27f, 0.5f};
    const Pht::Vec2 downArrowRenderableSize {0.6f, 0.6f};
    const Pht::Vec2 shadowOffset {0.05f, 0.05f};
    const Pht::Vec2 downArrowLineSize {0.13f, 0.3f};
    constexpr auto downArrowTriangleHeight = 0.195f;
    
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
    
    void DrawDownArrow(Pht::SoftwareRasterizer& rasterizer,
                   const Pht::Vec2& position,
                   const Pht::Vec4& color) {
        Pht::Vec2 lineLowerLeft {
            position.x - downArrowLineSize.x / 2.0f,
            position.y - downArrowLineSize.y / 2.0f
        };
        Pht::Vec2 lineUpperRight {
            position.x + downArrowLineSize.x / 2.0f,
            position.y + downArrowLineSize.y / 2.0f
        };
        rasterizer.DrawRectangle(lineUpperRight, lineLowerLeft, color, Pht::DrawOver::Yes);
        
        Pht::Vec2 leftTriangleUpperLeft {
            position.x - downArrowTriangleHeight,
            position.y - downArrowLineSize.y / 2.0f
        };
        Pht::Vec2 leftTriangleLowerRight {
            position.x,
            position.y - downArrowLineSize.y / 2.0f - downArrowTriangleHeight
        };
        rasterizer.DrawTiltedTrapezoid225(leftTriangleUpperLeft,
                                          leftTriangleLowerRight,
                                          downArrowTriangleHeight,
                                          color,
                                          Pht::DrawOver::Yes);

        Pht::Vec2 rightTriangleUpperRight {
            position.x + downArrowTriangleHeight,
            position.y - downArrowLineSize.y / 2.0f
        };
        Pht::Vec2 rightTriangleLowerLeft {
            position.x,
            position.y - downArrowLineSize.y / 2.0f - downArrowTriangleHeight
        };
        rasterizer.DrawTiltedTrapezoid315(rightTriangleUpperRight,
                                          rightTriangleLowerLeft,
                                          downArrowTriangleHeight,
                                          color,
                                          Pht::DrawOver::Yes);
    }
}

GameHudResources::GameHudResources(Pht::IEngine& engine, const CommonResources& commonResources) {
    CreateDownArrow(engine, commonResources);
    CreateArrowMesh(engine, commonResources);
}

void GameHudResources::CreateDownArrow(Pht::IEngine& engine, const CommonResources& commonResources) {
    auto rasterizer = CreateRasterizer(engine, commonResources, downArrowRenderableSize);
    
    DrawDownArrow(*rasterizer,
                  downArrowRenderableSize / 2.0f - shadowOffset + Pht::Vec2{0.0f, downArrowTriangleHeight / 2.0f},
                  shadowColor);
    DrawDownArrow(*rasterizer,
                  downArrowRenderableSize / 2.0f + Pht::Vec2{0.0f, downArrowTriangleHeight / 2.0f},
                  downArrowColor);
    
    auto image = rasterizer->ProduceImage();
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    Pht::QuadMesh quadMesh {downArrowRenderableSize.x, downArrowRenderableSize.y};
    mDownArrowRenderable = engine.GetSceneManager().CreateRenderableObject(quadMesh, imageMaterial);
}

void GameHudResources::CreateArrowMesh(Pht::IEngine& engine, const CommonResources& commonResources) {
    Pht::ObjMesh mesh {"arrow_428.obj", 3.2f};
    auto& blueMaterial = commonResources.GetMaterials().GetBlueArrowMaterial();
    mBlueArrowMeshRenderable =
        engine.GetSceneManager().CreateBatchableRenderableObject(mesh, blueMaterial);
    
    auto& yellowMaterial = commonResources.GetMaterials().GetYellowFieldBlockMaterial();
    mYellowArrowMeshRenderable =
        engine.GetSceneManager().CreateBatchableRenderableObject(mesh, yellowMaterial);
}
