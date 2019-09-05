#include "FieldBorder.hpp"

// Engine incudes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "SoftwareRasterizer.hpp"
#include "IImage.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "GameScene.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec4 redBorderColor {1.0f, 0.45f, 0.65f, 0.85f};
    const Pht::Vec4 redDarkerBorderColor {1.0f, 0.45f, 0.65f, 0.25f};
    const Pht::Vec4 blueBorderColor {0.4f, 0.6f, 1.0f, 0.85f};
    const Pht::Vec4 blueDarkerBorderColor {0.4f, 0.6f, 1.0f, 0.25f};
    constexpr auto defaultNumRows = 19;

    std::unique_ptr<Pht::SoftwareRasterizer> CreateRasterizer(Pht::IEngine& engine,
                                                              const Pht::Vec2& coordinateSystemSize,
                                                              const CommonResources& commonResources) {
        auto& renderer = engine.GetRenderer();
        auto& renderBufferSize = renderer.GetRenderBufferSize();
        auto& frustumSize = commonResources.GetOrthographicFrustumSizePotentiallyZoomedScreen();

        auto xScaleFactor =
            static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x);
    
        auto yScaleFactor =
            static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y);

        Pht::IVec2 imageSize {
            static_cast<int>(coordinateSystemSize.x * xScaleFactor),
            static_cast<int>(coordinateSystemSize.y * yScaleFactor)
        };

        return std::make_unique<Pht::SoftwareRasterizer>(coordinateSystemSize, imageSize);
    }
}

FieldBorder::FieldBorder(Pht::IEngine& engine,
                         GameScene& scene,
                         const CommonResources& commonResources) :
    mScene {scene} {
    
    auto defaultHeight = static_cast<float>(defaultNumRows) * scene.GetCellSize();
    CreateLeftBorder(engine, scene, commonResources, defaultHeight);
    CreateRightBorder(engine, scene, commonResources, defaultHeight);
}

void FieldBorder::Init(const Pht::Vec3& leftPosition, const Pht::Vec3& rightPosition, int numRows) {
    auto& container = mScene.GetFieldQuadContainer();
    container.AddChild(*mLeftBorder);
    container.AddChild(*mRightBorder);
    
    auto scale = static_cast<float>(numRows) / static_cast<float>(defaultNumRows);
    auto& leftTransform = mLeftBorder->GetTransform();
    leftTransform.SetScale({1.0f, scale, 1.0f});
    leftTransform.SetPosition(leftPosition);
    
    auto& rightTransform = mRightBorder->GetTransform();
    rightTransform.SetScale({1.0f, scale, 1.0f});
    rightTransform.SetPosition(rightPosition);
}

void FieldBorder::CreateLeftBorder(Pht::IEngine& engine,
                                   GameScene& scene,
                                   const CommonResources& commonResources,
                                   float defaultHeight) {
    Pht::Vec2 borderSize {borderThickness, defaultHeight};
    auto rasterizer = CreateRasterizer(engine, borderSize, commonResources);
    
    Pht::SoftwareRasterizer::VerticalGradientColors borderColors {blueBorderColor, redBorderColor};
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {brightBorderThickness, borderSize.y};
    rasterizer->DrawGradientRectangle(upperRight, lowerLeft, borderColors, Pht::DrawOver::Yes);

    Pht::SoftwareRasterizer::VerticalGradientColors darkerBorderColors {
        blueDarkerBorderColor,
        redDarkerBorderColor
    };
    Pht::Vec2 lowerLeft2 {brightBorderThickness, 0.0f};
    Pht::Vec2 upperRight2 {borderThickness, borderSize.y};
    rasterizer->DrawGradientRectangle(upperRight2,
                                      lowerLeft2,
                                      darkerBorderColors,
                                      Pht::DrawOver::Yes);

    auto image = rasterizer->ProduceImage();
    mLeftBorder = CreateSceneObject(*image, borderSize, engine);
}

void FieldBorder::CreateRightBorder(Pht::IEngine& engine,
                                    GameScene& scene,
                                    const CommonResources& commonResources,
                                    float defaultHeight) {
    Pht::Vec2 borderSize {borderThickness, defaultHeight};
    auto rasterizer = CreateRasterizer(engine, borderSize, commonResources);
    
    Pht::SoftwareRasterizer::VerticalGradientColors darkerBorderColors {
        redDarkerBorderColor,
        blueDarkerBorderColor
    };
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {darkerBorderThickness, borderSize.y};
    rasterizer->DrawGradientRectangle(upperRight,
                                      lowerLeft,
                                      darkerBorderColors,
                                      Pht::DrawOver::Yes);

    Pht::SoftwareRasterizer::VerticalGradientColors borderColors {redBorderColor, blueBorderColor};
    Pht::Vec2 lowerLeft2 {darkerBorderThickness, 0.0f};
    Pht::Vec2 upperRight2 {borderThickness, borderSize.y};
    rasterizer->DrawGradientRectangle(upperRight2, lowerLeft2, borderColors, Pht::DrawOver::Yes);

    auto image = rasterizer->ProduceImage();
    mRightBorder = CreateSceneObject(*image, borderSize, engine);
}

std::unique_ptr<Pht::SceneObject> FieldBorder::CreateSceneObject(const Pht::IImage& image,
                                                                 const Pht::Vec2& size,
                                                                 Pht::IEngine& engine) {
    Pht::Material imageMaterial {image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    auto& sceneManager = engine.GetSceneManager();
    return sceneManager.CreateSceneObject(Pht::QuadMesh {size.x, size.y},
                                          imageMaterial,
                                          mSceneResources);
}
