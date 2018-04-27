#include "GameHudView.hpp"

// Engine includes.
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "OfflineRasterizer.hpp"
#include "IImage.hpp"
#include "ISceneManager.hpp"
#include "UiLayer.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    std::unique_ptr<Pht::SceneObject> CreatePauseButtonSceneObject(Pht::OfflineRasterizer& rasterizer,
                                                                   Pht::IEngine& engine,
                                                                   const Pht::Vec2& coordinateSystemSize,
                                                                   float circleRadius,
                                                                   const Pht::Vec4& fillColor,
                                                                   Pht::SceneResources& resources) {
        Pht::Vec4 borderColor {1.0f, 1.0f, 1.0f, 0.95f};
        rasterizer.DrawCircle({circleRadius, circleRadius}, circleRadius - 0.05f, 0.05f, borderColor);
        
        rasterizer.FillEnclosedArea(fillColor);
        
        Pht::Vec2 leftBarLowerLeft {circleRadius - 0.24f, circleRadius - 0.33f};
        Pht::Vec2 leftBarUpperRight {circleRadius - 0.08f, circleRadius + 0.33f};
        rasterizer.DrawRectangle(leftBarUpperRight, leftBarLowerLeft, borderColor, Pht::DrawOver::Yes);

        Pht::Vec2 rightBarLowerLeft {circleRadius + 0.08f, circleRadius - 0.33f};
        Pht::Vec2 rightBarUpperRight {circleRadius + 0.24f, circleRadius + 0.33f};
        rasterizer.DrawRectangle(rightBarUpperRight, rightBarLowerLeft, borderColor, Pht::DrawOver::Yes);
        
        auto image {rasterizer.ProduceImage()};
        
        Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
        imageMaterial.SetBlend(Pht::Blend::Yes);
        
        auto& sceneManager {engine.GetSceneManager()};
        return sceneManager.CreateSceneObject(Pht::QuadMesh {coordinateSystemSize.x, coordinateSystemSize.y},
                                              imageMaterial,
                                              resources);
    }
}

GameHudView::GameHudView(Pht::IEngine& engine, const CommonResources& commonResources) {
    SetPosition({-5.2f, -11.85f});
    
    const auto circleRadius {0.85f};
    Pht::Vec2 coordinateSystemSize {circleRadius * 2.0f, circleRadius * 2.0f};
    auto& renderer {engine.GetRenderer()};

    auto& renderBufferSize {renderer.GetRenderBufferSize()};
    auto& frustumSize {commonResources.GetHudFrustumSizePotentiallyZoomedScreen()};
    
    auto xScaleFactor {static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)};
    auto yScaleFactor {static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)};
    
    Pht::IVec2 imageSize {
        static_cast<int>(coordinateSystemSize.x * xScaleFactor) * 2,
        static_cast<int>(coordinateSystemSize.y * yScaleFactor) * 2
    };
    
    auto rasterizer {std::make_unique<Pht::OfflineRasterizer>(coordinateSystemSize, imageSize)};
    
    Pht::Vec4 fillColor {0.75f, 0.35f, 0.85f, 0.63f};
    auto pauseButtonSceneObject {CreatePauseButtonSceneObject(*rasterizer,
                                                              engine,
                                                              coordinateSystemSize,
                                                              circleRadius,
                                                              fillColor,
                                                              GetSceneResources())};
    auto& pauseButtonSceneObjectCapture {*pauseButtonSceneObject};
    
    rasterizer->ClearBuffer();
    
    Pht::Vec4 pressedFillColor {0.95f, 0.6f, 0.95f, 0.63f};
    auto pressedPauseButtonSceneObject {CreatePauseButtonSceneObject(*rasterizer,
                                                                     engine,
                                                                     coordinateSystemSize,
                                                                     circleRadius,
                                                                     pressedFillColor,
                                                                     GetSceneResources())};
    pressedPauseButtonSceneObject->GetTransform().SetScale(1.35f);
    pressedPauseButtonSceneObject->SetIsVisible(false);
    auto& pressedPauseButtonSceneObjectCapture {*pressedPauseButtonSceneObject};

    auto buttonSceneObject {std::make_unique<Pht::SceneObject>(nullptr)};
    
    Pht::Vec2 buttonSize {60.0f, 60.0f};
    mPauseButton = std::make_unique<Pht::Button>(*buttonSceneObject, buttonSize, engine);
    buttonSceneObject->SetIsVisible(false);
    AddSceneObject(std::move(buttonSceneObject));
    
    auto pausePressedFunction {[&] () {
        pauseButtonSceneObjectCapture.SetIsVisible(false);
        pressedPauseButtonSceneObjectCapture.SetIsVisible(true);
    }};
    
    mPauseButton->SetOnDown(pausePressedFunction);
    
    auto pauseUnpressedFunction {[&] () {
        pauseButtonSceneObjectCapture.SetIsVisible(true);
        pressedPauseButtonSceneObjectCapture.SetIsVisible(false);
    }};
    
    mPauseButton->SetOnUpInside(pauseUnpressedFunction);
    mPauseButton->SetOnUpOutside(pauseUnpressedFunction);
    mPauseButton->SetOnMoveOutside(pauseUnpressedFunction);
    
    AddSceneObject(std::move(pauseButtonSceneObject));
    AddSceneObject(std::move(pressedPauseButtonSceneObject));
    
    auto switchButtonSceneObject {std::make_unique<Pht::SceneObject>(nullptr)};
    switchButtonSceneObject->SetIsVisible(false);
    switchButtonSceneObject->GetTransform().SetPosition({8.65f, 0.0f, UiLayer::root});
    
    Pht::Vec2 switchButtonSize {110.0f, 60.0f};
    mSwitchButton = std::make_unique<Pht::Button>(*switchButtonSceneObject, switchButtonSize, engine);
    
    AddSceneObject(std::move(switchButtonSceneObject));
}
