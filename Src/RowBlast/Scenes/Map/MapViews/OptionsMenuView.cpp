#include "OptionsMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

OptionsMenuView::OptionsMenuView(Pht::IEngine& engine, const CommonResources& commonResources) {
    auto zoom {PotentiallyZoomedScreen::No};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& sceneManager {engine.GetSceneManager()};

    Pht::Color gearColor {0.57f, 0.57f, 0.57f};
    Pht::Material gearMaterial {gearColor, gearColor, gearColor, 10.0f};
    gearMaterial.GetDepthState().mDepthTestAllowedOverride = true;
    auto& gearIcon {CreateSceneObject(Pht::ObjMesh {"gear_192.obj", 5.1f}, gearMaterial, sceneManager)};
    gearIcon.GetTransform().SetRotation({20.0f, 10.0f, 0.0f});
    gearIcon.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 1.4f, UiLayer::textRectangle});
    GetRoot().AddChild(gearIcon);
    
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.3f,
        GetSize().y / 2.0f - 1.3f,
        UiLayer::textRectangle
    };
    
    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};

    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);
    
    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);
        
    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    auto& buttonTextProperties {guiResources.GetWhiteButtonTextProperties(zoom)};
    auto settingsY {3.0f};
    
    MenuButton::Style settingsButtonStyle;
    settingsButtonStyle.mPressedScale = 1.05f;
    settingsButtonStyle.mRenderableObject = &guiResources.GetSmallBlueGlossyButton(zoom);
    settingsButtonStyle.mSelectedRenderableObject = &guiResources.GetSmallDarkBlueGlossyButton(zoom);

    Pht::Vec2 settingsButtonInputSize {78.0f, 43.0f};
    
    CreateText({-5.3f, 2.47f + settingsY, UiLayer::text}, "Music", textProperties);

    Pht::Vec3 musicButtonPosition {3.35f, 2.7f + settingsY, UiLayer::textRectangle};
    mMusicButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                musicButtonPosition,
                                                settingsButtonInputSize,
                                                settingsButtonStyle);
    mMusicOnText = &(mMusicButton->CreateText({-0.5f, -0.23f, UiLayer::buttonText},
                                              "On",
                                              buttonTextProperties).GetSceneObject());
    mMusicOffText = &(mMusicButton->CreateText({-0.6f, -0.23f, UiLayer::buttonText},
                                               "Off",
                                               buttonTextProperties).GetSceneObject());

    CreateText({-5.3f, 0.27f + settingsY, UiLayer::text}, "Sound Effects", textProperties);
    
    Pht::Vec3 soundButtonPosition {3.35f, 0.5f + settingsY, UiLayer::textRectangle};
    mSoundButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                soundButtonPosition,
                                                settingsButtonInputSize,
                                                settingsButtonStyle);
    mSoundButton->SetPlaySoundIfAudioDisabled(true);
    mSoundOnText = &(mSoundButton->CreateText({-0.5f, -0.23f, UiLayer::buttonText},
                                              "On",
                                              buttonTextProperties).GetSceneObject());
    mSoundOffText = &(mSoundButton->CreateText({-0.6f, -0.23f, UiLayer::buttonText},
                                               "Off",
                                               buttonTextProperties).GetSceneObject());

    CreateText({-5.3f, -1.93f + settingsY, UiLayer::text}, "Controls", textProperties);

    Pht::Vec3 controlsButtonPosition {3.35f, -1.7f + settingsY, UiLayer::textRectangle};
    mControlsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   controlsButtonPosition,
                                                   settingsButtonInputSize,
                                                   settingsButtonStyle);
    mControlsClickText = &(mControlsButton->CreateText({-1.7f, -0.23f, UiLayer::buttonText},
                                                       "SingleTap",
                                                       buttonTextProperties).GetSceneObject());
    mControlsSwipeText = &(mControlsButton->CreateText({-1.05f, -0.23f, UiLayer::buttonText},
                                                       "Swipe",
                                                       buttonTextProperties).GetSceneObject());

    MenuButton::Style buttonStyle;
    buttonStyle.mPressedScale = 1.05f;
    buttonStyle.mRenderableObject = &guiResources.GetMediumThinBlueGlossyButton();
    buttonStyle.mSelectedRenderableObject = &guiResources.GetMediumThinDarkBlueGlossyButton();

    Pht::Vec2 buttonInputSize {194.0f, 43.0f};

    mHowToPlayButton = std::make_unique<MenuButton>(engine,
                                                    *this,
                                                    Pht::Vec3 {0.0f, -1.6f, UiLayer::textRectangle},
                                                    buttonInputSize,
                                                    buttonStyle);
    mHowToPlayButton->CreateText({-2.2f, -0.23f, UiLayer::buttonText},
                                 "How To Play",
                                 buttonTextProperties);

    mAboutButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                Pht::Vec3 {0.0f, -3.8f, UiLayer::textRectangle},
                                                buttonInputSize,
                                                buttonStyle);
    mAboutButton->CreateText({-1.05f, -0.23f, UiLayer::buttonText}, "About", buttonTextProperties);

    mBackButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -8.25f, UiLayer::textRectangle},
                                               buttonInputSize,
                                               buttonStyle);
    mBackButton->CreateText({-0.85f, -0.23f, UiLayer::buttonText}, "Back", buttonTextProperties);
}
