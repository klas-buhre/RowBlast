#include "SettingsMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "CommonResources.hpp"

using namespace RowBlast;

SettingsMenuView::SettingsMenuView(Pht::IEngine& engine, const CommonResources& commonResources) :
    mCommonResources {commonResources} {

    auto zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetMediumDarkMenuWindow()};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-2.2f, 5.05f, UiLayer::text},
               "SETTINGS",
               guiResources.GetLargeWhiteTextProperties(zoom));
        
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.3f,
        GetSize().y / 2.0f - 1.25f,
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
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.5f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    auto& textProperties {guiResources.GetSmallWhiteTextProperties(zoom)};
    auto& buttonTextProperties {guiResources.GetWhiteButtonTextProperties(zoom)};

    MenuButton::Style settingsButtonStyle;
    settingsButtonStyle.mPressedScale = 1.05f;
    settingsButtonStyle.mRenderableObject = &guiResources.GetSmallBlueGlossyButton(zoom);
    settingsButtonStyle.mSelectedRenderableObject = &guiResources.GetSmallDarkBlueGlossyButton(zoom);

    Pht::Vec2 buttonInputSize {78.0f, 43.0f};
    
    Pht::Vec4 iconColor {1.0f, 1.0f, 1.0f, 1.0f};
    Pht::Vec4 iconShadowColor {0.2f, 0.2f, 0.2f, 0.5f};
    Pht::Vec3 iconShadowOffset {-0.05f, -0.05f, -0.1f};

    CreateIcon("music.png", {-5.55f, 2.72f, UiLayer::text}, {0.85f, 0.85f}, engine);
    CreateText({-4.8f, 2.47f, UiLayer::text}, "Music", textProperties);

    Pht::Vec3 musicButtonPosition {3.35f, 2.7f, UiLayer::textRectangle};
    mMusicButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                musicButtonPosition,
                                                buttonInputSize,
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

    CreateIcon("sound.png", {-5.5f, 0.52f, UiLayer::text}, {0.85f, 0.85f}, engine);
    CreateText({-4.8f, 0.27f, UiLayer::text}, "Sound Effects", textProperties);
    
    Pht::Vec3 soundButtonPosition {3.35f, 0.5f, UiLayer::textRectangle};
    mSoundButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                soundButtonPosition,
                                                buttonInputSize,
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

    CreateIcon("hand.png", {-5.55f, -1.68f, UiLayer::text}, {0.9f, 0.9f}, engine);
    CreateText({-4.8f, -1.93f, UiLayer::text}, "Controls", textProperties);

    Pht::Vec3 controlsButtonPosition {3.35f, -1.7f, UiLayer::textRectangle};
    mControlsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   controlsButtonPosition,
                                                   buttonInputSize,
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

    MenuButton::Style backButtonStyle;
    backButtonStyle.mPressedScale = 1.05f;
    backButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    backButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);

    Pht::Vec2 backButtonInputSize {194.0f, 43.0f};
    
    mBackButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -4.95f, UiLayer::textRectangle},
                                               backButtonInputSize,
                                               backButtonStyle);
    mBackButton->CreateIcon("back.png",
                            {-1.17f, 0.0f, UiLayer::buttonText},
                            {0.7f, 0.7f},
                            {1.0f, 1.0f, 1.0f, 1.0f},
                            Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                            Pht::Vec3 {-0.05f, -0.05f, -0.1f});
    mBackButton->CreateText({-0.46f, -0.23f, UiLayer::buttonText}, "Back", buttonTextProperties);
}

void SettingsMenuView::EnableControlsButton() {
    mIsControlsButtonEnabled = true;
    
    auto& blueButtonRenderable {
        mCommonResources.GetGuiResources().GetSmallBlueGlossyButton(PotentiallyZoomedScreen::Yes)
    };
    
    mControlsButton->GetSceneObject().SetRenderable(&blueButtonRenderable);
}

void SettingsMenuView::DisableControlsButton() {
    mIsControlsButtonEnabled = false;

    auto& grayButtonRenderable {
        mCommonResources.GetGuiResources().GetSmallGrayGlossyButtonPotentiallyZoomedScreen()
    };
    
    mControlsButton->GetSceneObject().SetRenderable(&grayButtonRenderable);
}

void SettingsMenuView::SetMusicOnIsVisible(bool isVisble) {
    mMusicOnText->SetIsVisible(isVisble);
    mMusicOnIcon->SetIsVisible(isVisble);
}

void SettingsMenuView::SetMusicOffIsVisible(bool isVisble) {
    mMusicOffText->SetIsVisible(isVisble);
    mMusicOffIcon->SetIsVisible(isVisble);
}

void SettingsMenuView::SetSoundOnIsVisible(bool isVisble) {
    mSoundOnText->SetIsVisible(isVisble);
    mSoundOnIcon->SetIsVisible(isVisble);
}

void SettingsMenuView::SetSoundOffIsVisible(bool isVisble) {
    mSoundOffText->SetIsVisible(isVisble);
    mSoundOffIcon->SetIsVisible(isVisble);
}

void SettingsMenuView::SetControlsClickIsVisible(bool isVisible) {
    mControlsClickText->SetIsVisible(isVisible);
    mControlsClickIcon1->SetIsVisible(isVisible);
    mControlsClickIcon2->SetIsVisible(isVisible);
    mControlsClickIcon3->SetIsVisible(isVisible);
}

void SettingsMenuView::SetControlsSwipeIsVisible(bool isVisible) {
    mControlsSwipeText->SetIsVisible(isVisible);
    mControlsSwipeIcon1->SetIsVisible(isVisible);
    mControlsSwipeIcon2->SetIsVisible(isVisible);
    mControlsSwipeIcon3->SetIsVisible(isVisible);
}

void SettingsMenuView::CreateIcon(const std::string& filename,
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
