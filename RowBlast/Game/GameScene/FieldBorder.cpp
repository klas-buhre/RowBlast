#include "FieldBorder.hpp"

// Engine incudes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "OfflineRasterizer.hpp"
#include "IImage.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto borderThickness {0.05f};
    // constexpr auto outerCornerRadius {0.17f};
    const Pht::Vec4 borderColor {0.3f, 0.6f, 1.0f, 0.75};
    const Pht::Vec4 darkerBorderColor {0.3f, 0.6f, 1.0f, 0.5f};

    std::unique_ptr<Pht::OfflineRasterizer> CreateRasterizer(Pht::IEngine& engine,
                                                             const Pht::Vec2& coordinateSystemSize,
                                                             const CommonResources& commonResources) {
        auto& renderer {engine.GetRenderer()};
        auto& renderBufferSize {renderer.GetRenderBufferSize()};
        auto& frustumSize {commonResources.GetOrthographicFrustumSizePotentiallyZoomedScreen()};

        auto xScaleFactor {
            static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)
        };
    
        auto yScaleFactor {
            static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)
        };

        Pht::IVec2 imageSize {
            static_cast<int>(coordinateSystemSize.x * xScaleFactor),
            static_cast<int>(coordinateSystemSize.y * yScaleFactor)
        };

        return std::make_unique<Pht::OfflineRasterizer>(coordinateSystemSize, imageSize);
    }
}

FieldBorder::FieldBorder(Pht::IEngine& engine,
                         GameScene& scene,
                         const CommonResources& commonResources,
                         float height) :
    mScene {scene} {
    
    CreateLeftBorder(engine, scene, commonResources, height);
    CreateRightBorder(engine, scene, commonResources, height);
}

void FieldBorder::Init() {
/*
    auto& fieldQuadContainer {mScene.GetFieldQuadContainer()};
    fieldQuadContainer.AddChild(*mLeftBorder);
    fieldQuadContainer.AddChild(*mRightBorder);
*/
}

void FieldBorder::CreateLeftBorder(Pht::IEngine& engine,
                                   GameScene& scene,
                                   const CommonResources& commonResources,
                                   float height) {
    Pht::Vec2 borderSize {borderThickness * 2.0f, height};
    auto rasterizer {CreateRasterizer(engine, borderSize, commonResources)};
    
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {borderThickness, borderSize.y};
    rasterizer->DrawRectangle(upperRight, lowerLeft, borderColor, Pht::DrawOver::Yes);

    Pht::Vec2 lowerLeft2 {borderThickness, 0.0f};
    Pht::Vec2 upperRight2 {borderThickness * 2.0f, borderSize.y};
    rasterizer->DrawRectangle(upperRight2, lowerLeft2, darkerBorderColor, Pht::DrawOver::Yes);

    auto image {rasterizer->ProduceImage()};
    mLeftBorder = CreateSceneObject(*image, borderSize, engine);
}

void FieldBorder::CreateRightBorder(Pht::IEngine& engine,
                                    GameScene& scene,
                                    const CommonResources& commonResources,
                                    float height) {
    Pht::Vec2 borderSize {borderThickness * 2.0f, height};
    auto rasterizer {CreateRasterizer(engine, borderSize, commonResources)};
    
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {borderThickness, borderSize.y};
    rasterizer->DrawRectangle(upperRight, lowerLeft, darkerBorderColor, Pht::DrawOver::Yes);

    Pht::Vec2 lowerLeft2 {borderThickness, 0.0f};
    Pht::Vec2 upperRight2 {borderThickness * 2.0f, borderSize.y};
    rasterizer->DrawRectangle(upperRight2, lowerLeft2, borderColor, Pht::DrawOver::Yes);

    auto image {rasterizer->ProduceImage()};
    mRightBorder = CreateSceneObject(*image, borderSize, engine);
}

std::unique_ptr<Pht::SceneObject> FieldBorder::CreateSceneObject(const Pht::IImage& image,
                                                                 const Pht::Vec2& size,
                                                                 Pht::IEngine& engine) {
    Pht::Material imageMaterial {image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    auto& sceneManager {engine.GetSceneManager()};
    return sceneManager.CreateSceneObject(Pht::QuadMesh {size.x, size.y},
                                          imageMaterial,
                                          mSceneResources);
}

void FieldBorder::SetPositions(const Pht::Vec3& left,
                               const Pht::Vec3& right,
                               const Pht::Vec3& bottom) {
    mLeftBorder->GetTransform().SetPosition(left);
    mRightBorder->GetTransform().SetPosition(right);
}
