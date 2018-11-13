#include "MapHudView.hpp"

// Engine includes.
#include "QuadMesh.hpp"
#include "Material.hpp"
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "SceneObject.hpp"
#include "ISceneManager.hpp"
#include "UiLayer.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

MapHudView::MapHudView(Pht::IEngine& engine, const CommonResources& commonResources) {
    auto& renderer {engine.GetRenderer()};
    auto& hudFrustumSize {renderer.GetHudFrustumSize()};
    auto& sceneManger {engine.GetSceneManager()};
    auto& guiResources {commonResources.GetGuiResources()};

    SetPosition({0.0f, 0.0f});

    Pht::Vec3 settingsButtonPosition {
        -hudFrustumSize.x / 2.0f + 1.5f,
        -hudFrustumSize.y / 2.0f + 1.5f + renderer.GetBottomPaddingHeight(),
        UiLayer::background
    };

    MenuButton::Style settingsButtonStyle;
    settingsButtonStyle.mPressedScale = 1.05f;
    settingsButtonStyle.mRenderableObject = &guiResources.GetSmallerBlueGlossyButton();
    settingsButtonStyle.mSelectedRenderableObject = &guiResources.GetSmallerDarkBlueGlossyButton();

    Pht::Vec2 settingsButtonInputSize {60.0f, 60.0f};
    mSettingsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   settingsButtonPosition,
                                                   settingsButtonInputSize,
                                                   settingsButtonStyle);

    Pht::Material gearIconMaterial {"settings.png"};
    gearIconMaterial.SetBlend(Pht::Blend::Yes);
    auto& gearIcon {CreateSceneObject(Pht::QuadMesh {1.35f, 1.35f}, gearIconMaterial, sceneManger)};
    mSettingsButton->GetSceneObject().AddChild(gearIcon);
    
    MenuButton::Style plusButtonStyle;
    plusButtonStyle.mPressedScale = 1.05f;
    plusButtonStyle.mTextScale = 1.4f;
    plusButtonStyle.mRenderableObject = &guiResources.GetSmallestBlueGlossyButton();
    plusButtonStyle.mSelectedRenderableObject = &guiResources.GetSmallestDarkBlueGlossyButton();
    
    Pht::Vec2 plusButtonInputSize {80.0f, 80.0f};
    
    Pht::TextProperties plusButtonTextProperties {
        commonResources.GetHussarFontSize35(PotentiallyZoomedScreen::No),
        1.4f,
        {1.0f, 1.0f, 1.0f, 1.0f}
    };
    
    Pht::Vec3 coinsButtonPosition {
        hudFrustumSize.x / 2.0f - 1.5f,
        hudFrustumSize.y / 2.0f - 1.15f - renderer.GetTopPaddingHeight(),
        UiLayer::root
    };
    
    mCoinsButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                coinsButtonPosition,
                                                plusButtonInputSize,
                                                plusButtonStyle);
    mCoinsButton->CreateText({-0.35f, -0.42f, UiLayer::buttonText}, "+", plusButtonTextProperties);

    Pht::Vec3 livesButtonPosition {
        -hudFrustumSize.x / 2.0f + 4.3f,
        hudFrustumSize.y / 2.0f - 1.15f - renderer.GetTopPaddingHeight(),
        UiLayer::root
    };
    
    mLivesButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                livesButtonPosition,
                                                plusButtonInputSize,
                                                plusButtonStyle);
    mLivesButton->CreateText({-0.35f, -0.42f, UiLayer::buttonText}, "+", plusButtonTextProperties);
}
