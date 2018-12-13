#include "SettingsMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

SettingsMenuView::SettingsMenuView(Pht::IEngine& engine,
                                   const CommonResources& commonResources,
                                   PotentiallyZoomedScreen zoom) :
    mCommonResources {commonResources},
    mZoom {zoom} {

    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-2.2f, 5.05f, UiLayer::text},
               "SETTINGS",
               guiResources.GetLargeWhiteTextProperties(zoom));
    
    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.5f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    auto& buttonTextProperties {guiResources.GetWhiteButtonTextProperties(zoom)};
    CreateText({-5.3f, 1.57f, UiLayer::text}, "Controls", textProperties);

    MenuButton::Style settingsButtonStyle;
    settingsButtonStyle.mPressedScale = 1.05f;
    settingsButtonStyle.mRenderableObject = &guiResources.GetSmallBlueGlossyButton(zoom);
    settingsButtonStyle.mSelectedRenderableObject = &guiResources.GetSmallDarkBlueGlossyButton(zoom);

    Pht::Vec2 buttonInputSize {78.0f, 43.0f};
    
    Pht::Vec3 controlsButtonPosition {3.45f, 1.8f, UiLayer::textRectangle};
    mControlsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   controlsButtonPosition,
                                                   buttonInputSize,
                                                   settingsButtonStyle);
    mControlsClickText = &(mControlsButton->CreateText({-0.85f, -0.23f, UiLayer::buttonText},
                                                       "Click",
                                                       buttonTextProperties).GetSceneObject());
    mControlsSwipeText = &(mControlsButton->CreateText({-1.05f, -0.23f, UiLayer::buttonText},
                                                       "Swipe",
                                                       buttonTextProperties).GetSceneObject());

    CreateText({-5.3f, -1.03f, UiLayer::text}, "Sound", textProperties);
    
    Pht::Vec3 soundButtonPosition {3.45f, -0.8f, UiLayer::textRectangle};
    mSoundButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                soundButtonPosition,
                                                buttonInputSize,
                                                settingsButtonStyle);
    mSoundOnText = &(mSoundButton->CreateText({-0.6f, -0.23f, UiLayer::buttonText},
                                              "On",
                                              buttonTextProperties).GetSceneObject());
    mSoundOffText = &(mSoundButton->CreateText({-0.7f, -0.23f, UiLayer::buttonText},
                                               "Off",
                                               buttonTextProperties).GetSceneObject());

    MenuButton::Style backButtonStyle;
    backButtonStyle.mPressedScale = 1.05f;
    backButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    backButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);

    Pht::Vec2 backButtonInputSize {194.0f, 43.0f};
    
    mBackButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -4.7f, UiLayer::textRectangle},
                                               backButtonInputSize,
                                               backButtonStyle);
    mBackButton->CreateText({-0.85f, -0.23f, UiLayer::buttonText}, "Back", buttonTextProperties);
}

void SettingsMenuView::EnableControlsButton() {
    mIsControlsButtonEnabled = true;
    
    auto& blueButtonRenderable {mCommonResources.GetGuiResources().GetSmallBlueGlossyButton(mZoom)};
    mControlsButton->GetSceneObject().SetRenderable(&blueButtonRenderable);
}

void SettingsMenuView::DisableControlsButton() {
    mIsControlsButtonEnabled = false;

    auto& grayButtonRenderable {
        mCommonResources.GetGuiResources().GetSmallGrayGlossyButtonPotentiallyZoomedScreen()
    };
    
    mControlsButton->GetSceneObject().SetRenderable(&grayButtonRenderable);
}
