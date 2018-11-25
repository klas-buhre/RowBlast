#include "GameHudView.hpp"

// Engine includes.
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "SceneObject.hpp"
#include "SoftwareRasterizer.hpp"
#include "IImage.hpp"
#include "ISceneManager.hpp"
#include "UiLayer.hpp"
#include "CommonResources.hpp"
#include "GameHudRectangles.hpp"

using namespace RowBlast;

namespace {
    float CalculateLowerHudObjectYPosition(const CommonResources& commonResources) {
        auto bottomPadding {commonResources.GetBottomPaddingPotentiallyZoomedScreen()};
        auto& frustumSize {commonResources.GetHudFrustumSizePotentiallyZoomedScreen()};
        auto yPosition {-frustumSize.y / 2.0f + bottomPadding + 1.35f};
        
        if (bottomPadding != 0.0f) {
            yPosition += 0.19f;
        }
        
        return yPosition;
    }
    
    std::unique_ptr<Pht::RenderableObject>
    CreatePauseBarsRenderable(Pht::IEngine& engine, const CommonResources& commonResources) {
        auto& frustumSize {commonResources.GetHudFrustumSizePotentiallyZoomedScreen()};
        const auto pauseBarsAreaSize {0.5f};
        Pht::Vec2 coordinateSystemSize {pauseBarsAreaSize * 2.0f, pauseBarsAreaSize * 2.0f};

        auto& renderer {engine.GetRenderer()};
        auto& renderBufferSize {renderer.GetRenderBufferSize()};
        
        auto xScaleFactor {
            static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)
        };
        
        auto yScaleFactor {
            static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)
        };
        
        Pht::IVec2 imageSize {
            static_cast<int>(coordinateSystemSize.x * xScaleFactor) * 2,
            static_cast<int>(coordinateSystemSize.y * yScaleFactor) * 2
        };
        
        auto rasterizer {
            std::make_unique<Pht::SoftwareRasterizer>(coordinateSystemSize, imageSize)
        };

        Pht::Vec4 barColor {0.3f, 0.15f, 0.355f, 0.65f};
        Pht::Vec4 barBorderColor {1.0f, 1.0f, 1.0f, 1.0f};

        Pht::Vec2 leftBarLowerLeft {pauseBarsAreaSize - 0.24f, pauseBarsAreaSize - 0.32f};
        Pht::Vec2 leftBarUpperRight {pauseBarsAreaSize - 0.10f, pauseBarsAreaSize + 0.32f};
        rasterizer->DrawRectangle(leftBarUpperRight,
                                  leftBarLowerLeft,
                                  barColor,
                                  Pht::DrawOver::Yes);

        Pht::Vec2 rightBarLowerLeft {pauseBarsAreaSize + 0.10f, pauseBarsAreaSize - 0.32f};
        Pht::Vec2 rightBarUpperRight {pauseBarsAreaSize + 0.24f, pauseBarsAreaSize + 0.32f};
        rasterizer->DrawRectangle(rightBarUpperRight,
                                  rightBarLowerLeft,
                                  barColor,
                                  Pht::DrawOver::Yes);

        auto image {rasterizer->ProduceImage()};

        Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
        imageMaterial.SetBlend(Pht::Blend::Yes);

        auto& sceneManager {engine.GetSceneManager()};
        return sceneManager.CreateRenderableObject(Pht::QuadMesh {coordinateSystemSize.x, coordinateSystemSize.y},
                                                   imageMaterial);
    }
}

GameHudView::GameHudView(Pht::IEngine& engine,
                         const CommonResources& commonResources,
                         const GameHudRectangles& hudRectangles) {
    Pht::Vec2 position {-6.6f, CalculateLowerHudObjectYPosition(commonResources)};
    SetPosition(position);

    auto pauseBarsRenderable {CreatePauseBarsRenderable(engine, commonResources)};
    auto& pauseButtonSceneObject {CreateSceneObject()};
    GetRoot().AddChild(pauseButtonSceneObject);

    auto& normalPauseButtonSceneObject {CreateSceneObject()};
    normalPauseButtonSceneObject.SetRenderable(&hudRectangles.GetPauseButtonRectangle());
    pauseButtonSceneObject.AddChild(normalPauseButtonSceneObject);
    auto& normalPauseButtonBarsSceneObject {CreateSceneObject()};
    normalPauseButtonBarsSceneObject.SetRenderable(pauseBarsRenderable.get());
    normalPauseButtonBarsSceneObject.GetTransform().SetPosition({0.2f, 0.0f, UiLayer::buttonText});
    normalPauseButtonSceneObject.AddChild(normalPauseButtonBarsSceneObject);

    auto& pressedPauseButtonSceneObject {CreateSceneObject()};
    pressedPauseButtonSceneObject.SetIsVisible(false);
    pressedPauseButtonSceneObject.SetRenderable(&hudRectangles.GetPressedPauseButtonRectangle());
    pressedPauseButtonSceneObject.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::textRectangle});
    pressedPauseButtonSceneObject.GetTransform().SetScale(1.1f);
    pauseButtonSceneObject.AddChild(pressedPauseButtonSceneObject);
    auto& pressedPauseButtonBarsSceneObject {CreateSceneObject()};
    pressedPauseButtonBarsSceneObject.SetRenderable(pauseBarsRenderable.get());
    pressedPauseButtonBarsSceneObject.GetTransform().SetPosition({0.2f, 0.0f, UiLayer::buttonText});
    pressedPauseButtonSceneObject.AddChild(pressedPauseButtonBarsSceneObject);

    Pht::Vec2 pauseButtonSize {55.0f, 55.0f};
    mPauseButton = std::make_unique<Pht::Button>(pauseButtonSceneObject, pauseButtonSize, engine);
    
    auto pausePressedFunction {[&] () {
        normalPauseButtonSceneObject.SetIsVisible(false);
        pressedPauseButtonSceneObject.SetIsVisible(true);
    }};
    
    mPauseButton->SetOnDown(pausePressedFunction);
    
    auto pauseUnpressedFunction {[&] () {
        normalPauseButtonSceneObject.SetIsVisible(true);
        pressedPauseButtonSceneObject.SetIsVisible(false);
    }};
    
    mPauseButton->SetOnUpInside(pauseUnpressedFunction);
    mPauseButton->SetOnUpOutside(pauseUnpressedFunction);
    mPauseButton->SetOnMoveOutside(pauseUnpressedFunction);

    auto& switchButtonSceneObject {CreateSceneObject()};
    GetRoot().AddChild(switchButtonSceneObject);
    Pht::Vec2 switchButtonSize {158.0f, 60.0f};
    mSwitchButton = std::make_unique<Pht::Button>(switchButtonSceneObject,
                                                  switchButtonSize,
                                                  engine);
    switchButtonSceneObject.SetIsVisible(false);
    switchButtonSceneObject.GetTransform().SetPosition({9.8f, 0.0f, UiLayer::root});

    GetSceneResources().AddRenderableObject(std::move(pauseBarsRenderable));
}
