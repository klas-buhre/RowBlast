#include "SettingsMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "MenuQuad.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

SettingsMenuView::SettingsMenuView(Pht::IEngine& engine,
                                   const CommonResources& commonResources,
                                   PotentiallyZoomedScreen zoom) {
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-2.0f, 5.0f, UiLayer::text},
               "SETTINGS",
               guiResources.GetCaptionTextProperties(zoom));
    
    auto& textProperties {guiResources.GetSmallTextProperties(zoom)};
    CreateText({-4.65f, 1.07f, UiLayer::text}, "Controls", textProperties);
    
    MenuButton::Style settingsButtonStyle;
    settingsButtonStyle.mMeshFilename = GuiResources::mMediumButtonMeshFilename;
    settingsButtonStyle.mColor = GuiResources::mBlueButtonColor;
    settingsButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    settingsButtonStyle.mPressedScale = 1.05f;
    settingsButtonStyle.mHasShadow = true;

    Pht::Vec2 buttonInputSize {86.0f, 43.0f};
    
    auto& buttonTextProperties {guiResources.GetWhiteButtonTextProperties(zoom)};
    
    Pht::Vec3 controlsButtonPosition {3.5f, 1.3f, UiLayer::textRectangle};
    mControlsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   controlsButtonPosition,
                                                   buttonInputSize,
                                                   settingsButtonStyle);
    mControlsClickText = &(mControlsButton->CreateText({-1.05f, -0.23f, UiLayer::buttonText},
                                                       "Click",
                                                       buttonTextProperties).GetSceneObject());
    mControlsSwipeText = &(mControlsButton->CreateText({-1.05f, -0.23f, UiLayer::buttonText},
                                                       "Swipe",
                                                       buttonTextProperties).GetSceneObject());

    CreateText({-4.65f, -1.53f, UiLayer::text}, "Sound", textProperties);
    
    Pht::Vec3 soundButtonPosition {3.5f, -1.3f, UiLayer::textRectangle};
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
    backButtonStyle.mMeshFilename = GuiResources::mMediumButtonMeshFilename;
    backButtonStyle.mColor = GuiResources::mBlueButtonColor;
    backButtonStyle.mSelectedColor = GuiResources::mBlueSelectedButtonColor;
    backButtonStyle.mPressedScale = 1.05f;
    backButtonStyle.mHasShadow = true;
    
    Pht::Vec2 backButtonInputSize {236.0f, 43.0f};
    
    mBackButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -3.9f, UiLayer::textRectangle},
                                               backButtonInputSize,
                                               backButtonStyle);
    mBackButton->CreateText({-1.0f, -0.23f, UiLayer::buttonText}, "BACK", buttonTextProperties);
}
