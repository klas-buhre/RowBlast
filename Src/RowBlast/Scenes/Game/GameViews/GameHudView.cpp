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
#include "GameHud.hpp"

using namespace RowBlast;

namespace {
    float CalculateLowerHudObjectYPosition(const CommonResources& commonResources) {
        auto bottomPadding = commonResources.GetBottomPaddingPotentiallyZoomedScreen();
        auto& frustumSize = commonResources.GetHudFrustumSizePotentiallyZoomedScreen();
        auto yPosition = -frustumSize.y / 2.0f + bottomPadding + 1.54f;
        if (bottomPadding == 0.0f) {
            yPosition += 1.05f;
        }
        
        return yPosition;
    }
}

GameHudView::GameHudView(Pht::IEngine& engine, const CommonResources& commonResources) {
    Pht::Vec2 position {0.0f, CalculateLowerHudObjectYPosition(commonResources)};
    SetPosition(position);

    auto& pauseButtonSceneObject = CreateSceneObject();
    auto bottomPadding = commonResources.GetBottomPaddingPotentiallyZoomedScreen();
    auto& frustumSize = commonResources.GetHudFrustumSizePotentiallyZoomedScreen();
    
    Pht::Vec3 pauseButtonPosition {
        bottomPadding == 0.0f ? -frustumSize.x / 2.0f + 0.8f : -6.6f, -0.2f, UiLayer::root
    };

    pauseButtonSceneObject.GetTransform().SetPosition(pauseButtonPosition);
    GetRoot().AddChild(pauseButtonSceneObject);

    auto& guiResources = commonResources.GetGuiResources();
    
    auto& normalPauseButtonSceneObject = CreateSceneObject();
    normalPauseButtonSceneObject.SetRenderable(&guiResources.GetBluePauseButtonPotentiallyZoomedScreen());
    normalPauseButtonSceneObject.GetTransform().SetPosition({0.1f, 0.0f, 0.0f});
    pauseButtonSceneObject.AddChild(normalPauseButtonSceneObject);

    auto& pressedPauseButtonSceneObject = CreateSceneObject();
    pressedPauseButtonSceneObject.SetIsVisible(false);
    pressedPauseButtonSceneObject.SetRenderable(&guiResources.GetDarkBluePauseButtonPotentiallyZoomedScreen());
    pressedPauseButtonSceneObject.GetTransform().SetPosition({0.1f, 0.0f, 0.0f});
    pressedPauseButtonSceneObject.GetTransform().SetScale(1.1f);
    pauseButtonSceneObject.AddChild(pressedPauseButtonSceneObject);

    Pht::Vec2 pauseButtonSize {55.0f, 55.0f};
    mPauseButton = std::make_unique<Pht::Button>(pauseButtonSceneObject, pauseButtonSize, engine);
    
    auto pausePressedFunction = [&] () {
        normalPauseButtonSceneObject.SetIsVisible(false);
        pressedPauseButtonSceneObject.SetIsVisible(true);
    };
    
    mPauseButton->SetOnDown(pausePressedFunction);
    
    auto pauseUnpressedFunction = [&] () {
        normalPauseButtonSceneObject.SetIsVisible(true);
        pressedPauseButtonSceneObject.SetIsVisible(false);
    };
    
    mPauseButton->SetOnUpInside(pauseUnpressedFunction);
    mPauseButton->SetOnUpOutside(pauseUnpressedFunction);
    mPauseButton->SetOnMoveOutside(pauseUnpressedFunction);

    auto& switchButtonSceneObject = CreateSceneObject();
    GetRoot().AddChild(switchButtonSceneObject);
    auto switchButtonSize = Pht::Vec2{180.0f, 60.0f} * GameHud::selectablePiecesContainerScale;
    mSwitchButton = std::make_unique<Pht::Button>(switchButtonSceneObject,
                                                  switchButtonSize,
                                                  engine);
    switchButtonSceneObject.SetIsVisible(false);
    switchButtonSceneObject.GetTransform().SetPosition({0.0, 0.0f, UiLayer::root});
}
