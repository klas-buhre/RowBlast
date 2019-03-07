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

    Pht::Vec2 settingsButtonInputSize {103.0f, 43.0f};
    
    Pht::Vec4 iconColor {1.0f, 1.0f, 1.0f, 1.0f};
    Pht::Vec4 iconShadowColor {0.2f, 0.2f, 0.2f, 0.5f};
    Pht::Vec3 iconShadowOffset {-0.05f, -0.05f, -0.1f};
    
    CreateIcon("music.png", {-5.55f, 2.72f + settingsY, UiLayer::text}, {0.85f, 0.85f}, engine);
    CreateText({-4.8f, 2.47f + settingsY, UiLayer::text}, "Music", textProperties);

    Pht::Vec3 musicButtonPosition {3.35f, 2.7f + settingsY, UiLayer::textRectangle};
    mMusicButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                musicButtonPosition,
                                                settingsButtonInputSize,
                                                settingsButtonStyle);
    mMusicOnIcon = &mMusicButton->CreateIcon("music.png",
                                             {-0.75f, 0.07f, UiLayer::buttonText},
                                             {0.85f, 0.85f},
                                             iconColor,
                                             iconShadowColor,
                                             iconShadowOffset);
    mMusicOnText = &(mMusicButton->CreateText({-0.1f, -0.23f, UiLayer::buttonText},
                                              "On",
                                              buttonTextProperties).GetSceneObject());
    mMusicOffIcon = &mMusicButton->CreateIcon("music_off.png",
                                              {-0.85f, 0.07f, UiLayer::buttonText},
                                              {0.85f, 0.85f},
                                              iconColor,
                                              iconShadowColor,
                                              iconShadowOffset);
    mMusicOffText = &(mMusicButton->CreateText({-0.2f, -0.23f, UiLayer::buttonText},
                                               "Off",
                                               buttonTextProperties).GetSceneObject());

    CreateIcon("sound.png", {-5.5f, 0.52f + settingsY, UiLayer::text}, {0.85f, 0.85f}, engine);
    CreateText({-4.8f, 0.27f + settingsY, UiLayer::text}, "Sound Effects", textProperties);
    
    Pht::Vec3 soundButtonPosition {3.35f, 0.5f + settingsY, UiLayer::textRectangle};
    mSoundButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                soundButtonPosition,
                                                settingsButtonInputSize,
                                                settingsButtonStyle);
    mSoundButton->SetPlaySoundIfAudioDisabled(true);
    mSoundOnIcon = &mSoundButton->CreateIcon("sound.png",
                                             {-0.7f, 0.07f, UiLayer::buttonText},
                                             {0.85f, 0.85f},
                                             iconColor,
                                             iconShadowColor,
                                             iconShadowOffset);
    mSoundOnText = &(mSoundButton->CreateText({-0.05f, -0.23f, UiLayer::buttonText},
                                              "On",
                                              buttonTextProperties).GetSceneObject());
    mSoundOffIcon = &mSoundButton->CreateIcon("sound_off.png",
                                              {-0.8f, 0.07f, UiLayer::buttonText},
                                              {0.85f, 0.85f},
                                              iconColor,
                                              iconShadowColor,
                                              iconShadowOffset);
    mSoundOffText = &(mSoundButton->CreateText({-0.15f, -0.23f, UiLayer::buttonText},
                                               "Off",
                                               buttonTextProperties).GetSceneObject());

    CreateIcon("hand.png", {-5.55f, -1.68f + settingsY, UiLayer::text}, {0.9f, 0.9f}, engine);
    CreateText({-4.8f, -1.93f + settingsY, UiLayer::text}, "Controls", textProperties);

    Pht::Vec3 controlsButtonPosition {3.35f, -1.7f + settingsY, UiLayer::textRectangle};
    mControlsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   controlsButtonPosition,
                                                   settingsButtonInputSize,
                                                   settingsButtonStyle);
    mControlsClickIcon1 = &mControlsButton->CreateIcon("hand.png",
                                                       {-1.85f, 0.03f, UiLayer::buttonText},
                                                       {0.9f, 0.9f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
    mControlsClickIcon2 = &mControlsButton->CreateIcon("circle.png",
                                                       {-1.88f, 0.42f, UiLayer::buttonText},
                                                       {0.36f, 0.36f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
    mControlsClickIcon3 = &mControlsButton->CreateIcon("circle.png",
                                                       {-1.88f, 0.42f, UiLayer::buttonText},
                                                       {0.42f, 0.42f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
    mControlsClickText = &(mControlsButton->CreateText({-1.25f, -0.23f, UiLayer::buttonText},
                                                       "SingleTap",
                                                       buttonTextProperties).GetSceneObject());
    
    
    mControlsSwipeText = &(mControlsButton->CreateText({-0.55f, -0.23f, UiLayer::buttonText},
                                                       "Swipe",
                                                       buttonTextProperties).GetSceneObject());
    mControlsSwipeIcon1 = &mControlsButton->CreateIcon("hand.png",
                                                       {-1.4f, 0.03f, UiLayer::buttonText},
                                                       {0.9f, 0.9f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
    mControlsSwipeIcon2 = &mControlsButton->CreateIcon("back.png",
                                                       {-1.8f, 0.4f, UiLayer::buttonText},
                                                       {0.42f, 0.42f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
    mControlsSwipeIcon3 = &mControlsButton->CreateIcon("right_arrow.png",
                                                       {-1.05f, 0.4f, UiLayer::buttonText},
                                                       {0.42f, 0.42f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);

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
    mBackButton->CreateIcon("home.png",
                            {-1.17f, 0.06f, UiLayer::buttonText},
                            {0.7f, 0.7f},
                            iconColor,
                            iconShadowColor,
                            iconShadowOffset);
    mBackButton->CreateText({-0.46f, -0.23f, UiLayer::buttonText}, "Back", buttonTextProperties);
}

void OptionsMenuView::SetMusicOnIsVisible(bool isVisble) {
    mMusicOnText->SetIsVisible(isVisble);
    mMusicOnIcon->SetIsVisible(isVisble);
}

void OptionsMenuView::SetMusicOffIsVisible(bool isVisble) {
    mMusicOffText->SetIsVisible(isVisble);
    mMusicOffIcon->SetIsVisible(isVisble);
}

void OptionsMenuView::SetSoundOnIsVisible(bool isVisble) {
    mSoundOnText->SetIsVisible(isVisble);
    mSoundOnIcon->SetIsVisible(isVisble);
}

void OptionsMenuView::SetSoundOffIsVisible(bool isVisble) {
    mSoundOffText->SetIsVisible(isVisble);
    mSoundOffIcon->SetIsVisible(isVisble);
}

void OptionsMenuView::SetControlsClickIsVisible(bool isVisible) {
    mControlsClickText->SetIsVisible(isVisible);
    mControlsClickIcon1->SetIsVisible(isVisible);
    mControlsClickIcon2->SetIsVisible(isVisible);
    mControlsClickIcon3->SetIsVisible(isVisible);
}

void OptionsMenuView::SetControlsSwipeIsVisible(bool isVisible) {
    mControlsSwipeText->SetIsVisible(isVisible);
    mControlsSwipeIcon1->SetIsVisible(isVisible);
    mControlsSwipeIcon2->SetIsVisible(isVisible);
    mControlsSwipeIcon3->SetIsVisible(isVisible);
}

void OptionsMenuView::CreateIcon(const std::string& filename,
                                 const Pht::Vec3& position,
                                 const Pht::Vec2& size,
                                 Pht::IEngine& engine) {
    Pht::Vec4 color {0.95f, 0.95f, 0.95f, 1.0f};
    Pht::Material iconMaterial {filename, 0.0f, 0.0f, 0.0f, 0.0f};
    iconMaterial.SetBlend(Pht::Blend::Yes);
    iconMaterial.SetOpacity(color.w);
    iconMaterial.SetAmbient(Pht::Color{color.x, color.y, color.z});
    
    auto& iconSceneObject {
        CreateSceneObject(Pht::QuadMesh {size.x, size.y}, iconMaterial, engine.GetSceneManager())
    };
    
    iconSceneObject.GetTransform().SetPosition(position);
    GetRoot().AddChild(iconSceneObject);
}
