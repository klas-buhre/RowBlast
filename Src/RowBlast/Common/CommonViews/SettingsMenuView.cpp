#include "SettingsMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "CommonResources.hpp"
#include "GuiUtils.hpp"

using namespace RowBlast;

SettingsMenuView::SettingsMenuView(Pht::IEngine& engine,
                                   const CommonResources& commonResources,
                                   SceneId sceneId) :
    mCommonResources {commonResources} {

    auto zoom = (sceneId == SceneId::Game ? PotentiallyZoomedScreen::Yes : PotentiallyZoomedScreen::No);
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetLargeDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    CreateText({-2.2f, 8.25f, UiLayer::text},
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
    auto& sceneManager = engine.GetSceneManager();
    auto& lineSceneObject =
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager);
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.5f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    auto& container = CreateSceneObject();
    container.GetTransform().SetPosition({0.0f, 3.2f, 0.0f});
    GetRoot().AddChild(container);
    
    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    auto& buttonTextProperties = guiResources.GetWhiteButtonTextProperties(zoom);

    MenuButton::Style settingsButtonStyle;
    settingsButtonStyle.mPressedScale = 1.05f;
    settingsButtonStyle.mRenderableObject = &guiResources.GetSmallBlueGlossyButton(zoom);
    settingsButtonStyle.mSelectedRenderableObject = &guiResources.GetSmallDarkBlueGlossyButton(zoom);

    Pht::Vec2 buttonInputSize {103.0f, 43.0f};
    
    Pht::Vec4 iconColor {1.0f, 1.0f, 1.0f, 1.0f};
    Pht::Vec4 iconShadowColor {0.2f, 0.2f, 0.2f, 0.5f};
    Pht::Vec3 iconShadowOffset {-0.05f, -0.05f, UiLayer::textShadow};

    GuiUtils::CreateIcon(engine,
                         *this,
                         "music.png",
                         {-5.55f, 2.72f, UiLayer::text},
                         {0.85f, 0.85f},
                         container);
    CreateText({-4.8f, 2.47f, UiLayer::text}, "Music", textProperties, container);

    Pht::Vec3 musicButtonPosition {3.45f, 2.7f, UiLayer::textRectangle};
    mMusicButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                container,
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

    GuiUtils::CreateIcon(engine,
                         *this,
                         "sound.png",
                         {-5.5f, 0.52f, UiLayer::text},
                         {0.85f, 0.85f},
                         container);
    CreateText({-4.8f, 0.27f, UiLayer::text}, "Sound Effects", textProperties, container);
    
    Pht::Vec3 soundButtonPosition {3.45f, 0.5f, UiLayer::textRectangle};
    mSoundButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                container,
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

    GuiUtils::CreateIcon(engine,
                         *this,
                         "hand.png",
                         {-5.55f, -1.68f, UiLayer::text},
                         {0.9f, 0.9f},
                         container);
    CreateText({-4.8f, -1.93f, UiLayer::text}, "Controls", textProperties, container);

    Pht::Vec3 controlsButtonPosition {3.45f, -1.7f, UiLayer::textRectangle};
    mControlsButton = std::make_unique<MenuButton>(engine,
                                                   *this,
                                                   container,
                                                   controlsButtonPosition,
                                                   buttonInputSize,
                                                   settingsButtonStyle);
    mDragControlsText = &(mControlsButton->CreateText({-1.55f, -0.23f, UiLayer::buttonText},
                                                      "Drag&Drop",
                                                      buttonTextProperties).GetSceneObject());
    mDragControlsIcon1 = &mControlsButton->CreateIcon("hand.png",
                                                      {-2.0f, -0.13f, UiLayer::buttonText},
                                                      {0.9f, 0.9f},
                                                      iconColor,
                                                      iconShadowColor,
                                                      iconShadowOffset);
    auto smallBlockOffset = 0.2f;
    Pht::Vec2 smallBlockSize {0.19f, 0.19f};
    mDragControlsIcon2 = &CreateLPieceIcon(engine,
                                           *mControlsButton,
                                           {-1.93f, 0.43f, UiLayer::buttonText},
                                           smallBlockSize,
                                           smallBlockOffset,
                                           iconColor,
                                           {0.2f, 0.2f, 0.2f, 0.6f},
                                           {-0.05f, 0.05f, UiLayer::textShadow},
                                           90.0f);

    mClickControlsText = &(mControlsButton->CreateText({-1.25f, -0.23f, UiLayer::buttonText},
                                                       "SingleTap",
                                                       buttonTextProperties).GetSceneObject());
    mClickControlsIcon1 = &mControlsButton->CreateIcon("hand.png",
                                                       {-1.85f, 0.03f, UiLayer::buttonText},
                                                       {0.9f, 0.9f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
    mClickControlsIcon2 = &mControlsButton->CreateIcon("circle.png",
                                                       {-1.88f, 0.42f, UiLayer::buttonText},
                                                       {0.36f, 0.36f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
    mClickControlsIcon3 = &mControlsButton->CreateIcon("circle.png",
                                                       {-1.88f, 0.42f, UiLayer::buttonText},
                                                       {0.42f, 0.42f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
                
    mSwipeControlsText = &(mControlsButton->CreateText({-0.7f, -0.23f, UiLayer::buttonText},
                                                       "Swipe",
                                                       buttonTextProperties).GetSceneObject());
    mSwipeControlsIcon1 = &mControlsButton->CreateIcon("hand.png",
                                                       {-1.4f, 0.03f, UiLayer::buttonText},
                                                       {0.9f, 0.9f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
    mSwipeControlsIcon2 = &mControlsButton->CreateIcon("back.png",
                                                       {-1.8f, 0.4f, UiLayer::buttonText},
                                                       {0.42f, 0.42f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);
    mSwipeControlsIcon3 = &mControlsButton->CreateIcon("right_arrow.png",
                                                       {-1.05f, 0.4f, UiLayer::buttonText},
                                                       {0.42f, 0.42f},
                                                       iconColor,
                                                       iconShadowColor,
                                                       iconShadowOffset);

    auto blockOffset = 0.4f;
    Pht::Vec2 blockSize {0.39f, 0.39f};
    CreateLPieceIcon(engine, container, {-5.5f, -3.88f, UiLayer::text}, blockSize, blockOffset);
    CreateText({-4.8f, -4.13f, UiLayer::text}, "Swipe Aim Piece", textProperties, container);
    
    Pht::Vec3 ghostPieceButtonPosition {3.45f, -3.9f, UiLayer::textRectangle};
    mGhostPieceButton = std::make_unique<MenuButton>(engine,
                                                     *this,
                                                     container,
                                                     ghostPieceButtonPosition,
                                                     buttonInputSize,
                                                     settingsButtonStyle);
    CreateLPieceIcon(engine,
                     *mGhostPieceButton,
                     {-0.7f, 0.07f, UiLayer::buttonText},
                     blockSize,
                     blockOffset,
                     iconColor,
                     iconShadowColor,
                     iconShadowOffset,
                     0.0f);

    mGhostPieceDisabledIcon = &mGhostPieceButton->CreateIcon("disable.png",
                                                             {-0.6f, -0.03f, UiLayer::buttonText},
                                                             {1.2f, 1.2f},
                                                             iconColor,
                                                             iconShadowColor,
                                                             Pht::Vec3{-0.0f, -0.2f, UiLayer::textShadow});
    mGhostPieceDisabledIcon->GetTransform().SetScale({1.0f, 0.3f, 1.0f});
    mGhostPieceDisabledIcon->GetTransform().SetRotation({0.0f, 0.0f, -45.0f});
    mGhostPieceOnText = &mGhostPieceButton->CreateText({-0.05f, -0.23f, UiLayer::buttonText},
                                                       "On",
                                                       buttonTextProperties).GetSceneObject();

    mGhostPieceOffText = &mGhostPieceButton->CreateText({-0.05f, -0.23f, UiLayer::buttonText},
                                                        "Off",
                                                        buttonTextProperties).GetSceneObject();

    CreateText({-4.8f, -8.53f, UiLayer::text}, "Clear Effect", textProperties, container);

    Pht::Vec3 clearEffectButtonPosition {3.45f, -8.3f, UiLayer::textRectangle};
    mClearEffectButton = std::make_unique<MenuButton>(engine,
                                                      *this,
                                                      container,
                                                      clearEffectButtonPosition,
                                                      buttonInputSize,
                                                      settingsButtonStyle);
    mShrinkClearEffectText = &mClearEffectButton->CreateText({-0.7f, -0.23f, UiLayer::buttonText},
                                                             "Shrink",
                                                             buttonTextProperties).GetSceneObject();
    mFlyClearEffectText = &mClearEffectButton->CreateText({-0.05f, -0.23f, UiLayer::buttonText},
                                                          "Fly",
                                                          buttonTextProperties).GetSceneObject();

    MenuButton::Style backButtonStyle;
    backButtonStyle.mPressedScale = 1.05f;
    backButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    backButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);

    Pht::Vec2 backButtonInputSize {194.0f, 43.0f};
    
    mBackButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -8.0f, UiLayer::textRectangle},
                                               backButtonInputSize,
                                               backButtonStyle);
    switch (sceneId) {
        case SceneId::Game:
            mBackButton->CreateIcon("back.png",
                                    {-1.17f, 0.0f, UiLayer::buttonText},
                                    {0.7f, 0.7f},
                                    iconColor,
                                    iconShadowColor,
                                    iconShadowOffset);
            break;
        case SceneId::Map:
            mBackButton->CreateIcon("home.png",
                                    {-1.17f, 0.06f, UiLayer::buttonText},
                                    {0.7f, 0.7f},
                                    iconColor,
                                    iconShadowColor,
                                    iconShadowOffset);
            break;
    }

    mBackButton->CreateText({-0.46f, -0.23f, UiLayer::buttonText}, "Back", buttonTextProperties);
}

void SettingsMenuView::CreateLPieceIcon(Pht::IEngine& engine,
                                        Pht::SceneObject& parent,
                                        const Pht::Vec3& position,
                                        const Pht::Vec2& blockSize,
                                        float blockOffset) {
    GuiUtils::CreateIcon(engine,
                         *this,
                         "block.png",
                         position + Pht::Vec3{blockOffset / 2.0f, blockOffset / 2.0f, 0.0f},
                         blockSize,
                         parent);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "block.png",
                         position + Pht::Vec3{-blockOffset / 2.0f, -blockOffset / 2.0f, 0.0f},
                         blockSize,
                         parent);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "block.png",
                         position + Pht::Vec3{blockOffset / 2.0f, -blockOffset / 2.0f, 0.0f},
                         blockSize,
                         parent);
}

Pht::SceneObject& SettingsMenuView::CreateLPieceIcon(Pht::IEngine& engine,
                                                     MenuButton& button,
                                                     const Pht::Vec3& position,
                                                     const Pht::Vec2& blockSize,
                                                     float blockOffset,
                                                     const Pht::Vec4& color,
                                                     const Pht::Vec4& shadowColor,
                                                     const Pht::Vec3& shadowOffset,
                                                     float angle) {
    auto& iconContainer = CreateSceneObject();
    button.GetSceneObject().AddChild(iconContainer);

    GuiUtils::CreateIconWithShadow(engine,
                                   GetSceneResources(),
                                   "block.png",
                                   {blockOffset / 2.0f, blockOffset / 2.0f, 0.0f},
                                   blockSize,
                                   iconContainer,
                                   color,
                                   shadowColor,
                                   shadowOffset);
    GuiUtils::CreateIconWithShadow(engine,
                                   GetSceneResources(),
                                   "block.png",
                                   {-blockOffset / 2.0f, -blockOffset / 2.0f, 0.0f},
                                   blockSize,
                                   iconContainer,
                                   color,
                                   shadowColor,
                                   shadowOffset);
    GuiUtils::CreateIconWithShadow(engine,
                                   GetSceneResources(),
                                   "block.png",
                                   {blockOffset / 2.0f, -blockOffset / 2.0f, 0.0f},
                                   blockSize,
                                   iconContainer,
                                   color,
                                   shadowColor,
                                   shadowOffset);
    
    auto& transform = iconContainer.GetTransform();
    transform.SetPosition(position);
    transform.SetRotation({0.0f, 0.0f, angle});
    return iconContainer;
}

void SettingsMenuView::EnableControlsButton() {
    mIsControlsButtonEnabled = true;
    
    auto& blueButtonRenderable =
        mCommonResources.GetGuiResources().GetSmallBlueGlossyButton(PotentiallyZoomedScreen::Yes);
    
    mControlsButton->GetSceneObject().SetRenderable(&blueButtonRenderable);
}

void SettingsMenuView::DisableControlsButton() {
    mIsControlsButtonEnabled = false;

    auto& grayButtonRenderable =
        mCommonResources.GetGuiResources().GetSmallGrayGlossyButtonPotentiallyZoomedScreen();
    
    mControlsButton->GetSceneObject().SetRenderable(&grayButtonRenderable);
}

void SettingsMenuView::SetMusicIsOn(bool musicIsOn) {
    SetMusicOnIsVisible(musicIsOn);
    SetMusicOffIsVisible(!musicIsOn);
}

void SettingsMenuView::SetSoundIsOn(bool soundIsOn) {
    SetSoundOnIsVisible(soundIsOn);
    SetSoundOffIsVisible(!soundIsOn);
}

void SettingsMenuView::EnableDragControls() {
    SetDragControlsIsVisible(true);
    SetClickControlsIsVisible(false);
    SetSwipeControlsIsVisible(false);
}

void SettingsMenuView::EnableClickControls() {
    SetDragControlsIsVisible(false);
    SetClickControlsIsVisible(true);
    SetSwipeControlsIsVisible(false);
}

void SettingsMenuView::EnableSwipeControls() {
    SetDragControlsIsVisible(false);
    SetClickControlsIsVisible(false);
    SetSwipeControlsIsVisible(true);
}

void SettingsMenuView::SetGhostPieceIsEnabled(bool ghostPieceIsEnabled) {
    SetGhostPieceOnIsVisible(ghostPieceIsEnabled);
    SetGhostPieceOffIsVisible(!ghostPieceIsEnabled);
}

void SettingsMenuView::SetMusicOnIsVisible(bool isVisible) {
    mMusicOnText->SetIsVisible(isVisible);
    mMusicOnIcon->SetIsVisible(isVisible);
}

void SettingsMenuView::SetMusicOffIsVisible(bool isVisible) {
    mMusicOffText->SetIsVisible(isVisible);
    mMusicOffIcon->SetIsVisible(isVisible);
}

void SettingsMenuView::SetSoundOnIsVisible(bool isVisible) {
    mSoundOnText->SetIsVisible(isVisible);
    mSoundOnIcon->SetIsVisible(isVisible);
}

void SettingsMenuView::SetSoundOffIsVisible(bool isVisible) {
    mSoundOffText->SetIsVisible(isVisible);
    mSoundOffIcon->SetIsVisible(isVisible);
}

void SettingsMenuView::SetDragControlsIsVisible(bool isVisible) {
    mDragControlsText->SetIsVisible(isVisible);
    mDragControlsIcon1->SetIsVisible(isVisible);
    mDragControlsIcon2->SetIsVisible(isVisible);
}

void SettingsMenuView::SetClickControlsIsVisible(bool isVisible) {
    mClickControlsText->SetIsVisible(isVisible);
    mClickControlsIcon1->SetIsVisible(isVisible);
    mClickControlsIcon2->SetIsVisible(isVisible);
    mClickControlsIcon3->SetIsVisible(isVisible);
}

void SettingsMenuView::SetSwipeControlsIsVisible(bool isVisible) {
    mSwipeControlsText->SetIsVisible(isVisible);
    mSwipeControlsIcon1->SetIsVisible(isVisible);
    mSwipeControlsIcon2->SetIsVisible(isVisible);
    mSwipeControlsIcon3->SetIsVisible(isVisible);
}

void SettingsMenuView::SetGhostPieceOnIsVisible(bool isVisible) {
    mGhostPieceOnText->SetIsVisible(isVisible);
    mGhostPieceDisabledIcon->SetIsVisible(!isVisible);
}

void SettingsMenuView::SetGhostPieceOffIsVisible(bool isVisible) {
    mGhostPieceOffText->SetIsVisible(isVisible);
    mGhostPieceDisabledIcon->SetIsVisible(isVisible);
}

void SettingsMenuView::EnableFlyClearEffect() {
    mFlyClearEffectText->SetIsVisible(true);
    mShrinkClearEffectText->SetIsVisible(false);
}

void SettingsMenuView::EnableShrinkClearEffect() {
    mShrinkClearEffectText->SetIsVisible(true);
    mFlyClearEffectText->SetIsVisible(false);
}
