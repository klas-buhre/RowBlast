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
                                   PotentiallyZoomedScreen zoom) :
    mCommonResources {commonResources} {

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
    container.GetTransform().SetPosition({0.0f, 3.6f, 0.0f});
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

    auto blockOffset = 0.4f;
    Pht::Vec2 blockSize {0.39f, 0.39f};
    CreateLPieceIcon(engine, container, {-5.5f, -1.68f, UiLayer::text}, blockSize, blockOffset, 0.0f);
    CreateText({-4.8f, -1.93f, UiLayer::text}, "Swipe Aim Piece", textProperties, container);
    
    Pht::Vec3 ghostPieceButtonPosition {3.45f, -1.7f, UiLayer::textRectangle};
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

    auto& line2SceneObject =
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 1.5f, 0.06f}, lineMaterial, sceneManager);
    line2SceneObject.GetTransform().SetPosition({0.0f, -2.9f, UiLayer::textRectangle});
    container.AddChild(line2SceneObject);
    
    mControlsSection = &CreateSceneObject();
    GetRoot().AddChild(*mControlsSection);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "hand.png",
                         {-5.55f, -0.28f, UiLayer::text},
                         {0.9f, 0.9f},
                         *mControlsSection);
    CreateText({-4.8f, -0.53f, UiLayer::text}, "Controls:", textProperties, *mControlsSection);

    auto centeredTextProperties = textProperties;
    centeredTextProperties.mAlignment = Pht::TextAlignment::CenterX;

    Pht::Vec2 controlsInputSize {80.0f, 80.0f};
    auto& dragAndDropSection = CreateSceneObject();
    dragAndDropSection.GetTransform().SetPosition({-3.9f, -3.0f, UiLayer::root});
    mControlsSection->AddChild(dragAndDropSection);
    
    auto smallBlockOffset = 0.2f;
    Pht::Vec2 smallBlockSize {0.19f, 0.19f};

    GuiUtils::CreateIcon(engine,
                         *this,
                         "hand.png",
                         {0.0f, 1.0f, UiLayer::buttonText},
                         {0.9f, 0.9f},
                         dragAndDropSection);
    CreateLPieceIcon(engine,
                     dragAndDropSection,
                     {0.07f, 1.56f, UiLayer::text},
                     smallBlockSize,
                     smallBlockOffset,
                     90.0f);

    CreateText({0.0f, -0.15f, UiLayer::text}, "Drag&Drop", centeredTextProperties, dragAndDropSection);
    
    mDragAndDropButton = std::make_unique<RadioButton>(engine,
                                                       *this,
                                                       dragAndDropSection,
                                                       Pht::Vec3 {0.0f, -1.0f, 0.0f},
                                                       controlsInputSize);

    auto& swipeSection = CreateSceneObject();
    swipeSection.GetTransform().SetPosition({0.0f, -3.0f, UiLayer::root});
    mControlsSection->AddChild(swipeSection);

    GuiUtils::CreateIcon(engine,
                         *this,
                         "hand.png",
                         {0.0f, 1.0f, UiLayer::buttonText},
                         {0.9f, 0.9f},
                         swipeSection);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "back.png",
                         {-0.4f, 1.37f, UiLayer::buttonText},
                         {0.42f, 0.42f},
                         swipeSection);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "right_arrow.png",
                         {0.35f, 1.37f, UiLayer::buttonText},
                         {0.42f, 0.42f},
                         swipeSection);

    CreateText({0.0f, -0.15f, UiLayer::text}, "Swipe", centeredTextProperties, swipeSection);

    mSwipeButton = std::make_unique<RadioButton>(engine,
                                                 *this,
                                                 swipeSection,
                                                 Pht::Vec3 {0.0f, -1.0f, 0.0f},
                                                 controlsInputSize);
 
    auto& singleTapSection = CreateSceneObject();
    singleTapSection.GetTransform().SetPosition({3.9f, -3.0f, UiLayer::root});
    mControlsSection->AddChild(singleTapSection);

    GuiUtils::CreateIcon(engine,
                         *this,
                         "hand.png",
                         {0.0f, 1.0f, UiLayer::buttonText},
                         {0.9f, 0.9f},
                         singleTapSection);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "circle.png",
                         {-0.03f, 1.39f, UiLayer::buttonText},
                         {0.36f, 0.36f},
                         singleTapSection);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "circle.png",
                         {-0.03f, 1.39f, UiLayer::buttonText},
                         {0.42f, 0.42f},
                         singleTapSection);

    CreateText({0.0f, -0.15f, UiLayer::text}, "SingleTap", centeredTextProperties, singleTapSection);

    mSingleTapButton = std::make_unique<RadioButton>(engine,
                                                     *this,
                                                     singleTapSection,
                                                     Pht::Vec3 {0.0f, -1.0f, 0.0f},
                                                     controlsInputSize);





    CreateText({-4.8f, -9.53f, UiLayer::text}, "Clear Effect", textProperties, container);

    Pht::Vec3 clearEffectButtonPosition {3.45f, -9.3f, UiLayer::textRectangle};
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
}

void SettingsMenuView::CreateLPieceIcon(Pht::IEngine& engine,
                                        Pht::SceneObject& parent,
                                        const Pht::Vec3& position,
                                        const Pht::Vec2& blockSize,
                                        float blockOffset,
                                        float angle) {
    auto& iconContainer = CreateSceneObject();
    parent.AddChild(iconContainer);
    
    GuiUtils::CreateIcon(engine,
                         *this,
                         "block.png",
                         {blockOffset / 2.0f, blockOffset / 2.0f, 0.0f},
                         blockSize,
                         iconContainer);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "block.png",
                         {-blockOffset / 2.0f, -blockOffset / 2.0f, 0.0f},
                         blockSize,
                         iconContainer);
    GuiUtils::CreateIcon(engine,
                         *this,
                         "block.png",
                         {blockOffset / 2.0f, -blockOffset / 2.0f, 0.0f},
                         blockSize,
                         iconContainer);

    auto& transform = iconContainer.GetTransform();
    transform.SetPosition(position);
    transform.SetRotation({0.0f, 0.0f, angle});
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

void SettingsMenuView::EnableControlsSection() {
    mIsControlsSectionEnabled = true;
    mControlsSection->SetIsVisible(true);
}

void SettingsMenuView::DisableControlsSection() {
    mIsControlsSectionEnabled = false;
    mControlsSection->SetIsVisible(false);
}

void SettingsMenuView::SetMusicIsOn(bool musicIsOn) {
    SetMusicOnIsVisible(musicIsOn);
    SetMusicOffIsVisible(!musicIsOn);
}

void SettingsMenuView::SetSoundIsOn(bool soundIsOn) {
    SetSoundOnIsVisible(soundIsOn);
    SetSoundOffIsVisible(!soundIsOn);
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

void SettingsMenuView::DeselectAllControlButtons() {
    mDragAndDropButton->SetIsSelected(false);
    mSwipeButton->SetIsSelected(false);
    mSingleTapButton->SetIsSelected(false);
}
