#include "LaserDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "IAnimationSystem.hpp"
#include "Animation.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "PieceResources.hpp"
#include "LevelResources.hpp"
#include "UserServices.hpp"
#include "GuiUtils.hpp"
#include "TutorialUtils.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration = 5.0f;
    constexpr auto clickMoveTime = 1.5f;
    constexpr auto rowBombPosition2Time = 1.7f;
    constexpr auto detonationTime = 2.1f;
    constexpr auto rowBombRotationSpeed = 35.0f;
}

LaserDialogView::LaserDialogView(Pht::IEngine& engine,
                                 const CommonResources& commonResources,
                                 const PieceResources& pieceResources,
                                 const LevelResources& levelResources,
                                 const UserServices& userServices) :
    mEngine {engine},
    mUserServices {userServices} {

    PotentiallyZoomedScreen zoom {PotentiallyZoomedScreen::Yes};
    auto& guiResources = commonResources.GetGuiResources();
    auto& menuWindow = guiResources.GetLargeDarkMenuWindow();
    
    auto menuWindowSceneObject = std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable());
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    SetSize(menuWindow.GetSize());
    
    auto& largeTextProperties = guiResources.GetLargeWhiteTextProperties(zoom);
    CreateText({-1.4f, 8.25f, UiLayer::text}, "LASER", largeTextProperties);

    GuiUtils::CreateTitleBarLine(engine, *this);
    
    CreateAnimation(pieceResources, levelResources);
    
    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-3.75f, -5.3f, UiLayer::text}, "The laser clears a row", textProperties);
    
    Pht::Vec2 playButtonInputSize {194.0f, 43.0f};

    MenuButton::Style playButtonStyle;
    playButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    playButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);
    playButtonStyle.mPressedScale = 1.05f;

    mPlayButton = std::make_unique<MenuButton>(engine,
                                               *this,
                                               Pht::Vec3 {0.0f, -8.2f, UiLayer::textRectangle},
                                               playButtonInputSize,
                                               playButtonStyle);
    mPlayButton->CreateText({-0.9f, -0.23f, UiLayer::buttonText},
                            "PLAY",
                            guiResources.GetWhiteButtonTextProperties(zoom));
}

void LaserDialogView::CreateAnimation(const PieceResources& pieceResources,
                                      const LevelResources& levelResources) {
    auto& container = CreateSceneObject();
    container.GetTransform().SetPosition({0.0f, 1.5f, 0.0f});
    container.GetTransform().SetScale(1.15f);
    GetRoot().AddChild(container);
    
    auto& animationSystem = mEngine.GetAnimationSystem();
    mAnimation = &animationSystem.CreateAnimation(container, {{.mTime = 0.0f}, {.mTime = animationDuration}});

    TutorialUtils::CreateFieldQuad(mEngine, *this, container);
    TutorialUtils::CreateFieldQuadFrame(mEngine, *this, container);

    auto& coloredBlocks = CreateSceneObject();
    coloredBlocks.GetTransform().SetPosition({2.0f, 0.0f, 0.0f});
    container.AddChild(coloredBlocks);

    TutorialUtils::CreateColoredBlock(*this, {1.0f, 1.0f, 0.0f}, BlockColor::Green, coloredBlocks, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {-1.0f, -0.5f, 0.0f}, BlockColor::Blue, 90.0f, coloredBlocks, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {0.0f, -0.5f, 0.0f}, BlockColor::Yellow, 90.0f, coloredBlocks, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {1.0f, -0.5f, 0.0f}, BlockColor::Red, 90.0f, coloredBlocks, pieceResources);
    
    TutorialUtils::CreateGrayBlock(*this, {-3.0f, -2.0f, 0.0f}, container, levelResources);
    TutorialUtils::CreateGrayBlock(*this, {1.0f, -2.0f, 0.0f}, container, levelResources);
    TutorialUtils::CreateGrayBlock(*this, {2.0f, -2.0f, 0.0f}, container, levelResources);
    TutorialUtils::CreateGrayBlock(*this, {3.0f, -2.0f, 0.0f}, container, levelResources);
    TutorialUtils::CreateThreeGrayBlocks(*this, {-2.0f, -3.0f, 0.0f}, container, levelResources);
    TutorialUtils::CreateThreeGrayBlocks(*this, {-2.0f, -4.0f, 0.0f}, container, levelResources);
    
    auto& rowBombMove1 = TutorialUtils::CreateTransparentRowBomb(*this, {-2.0f, -2.0f, 0.0f}, container, pieceResources);
    auto& rowBombMove2 = TutorialUtils::CreateTransparentRowBomb(*this, {2.0f, 1.0f, 0.0f}, container, pieceResources);
    auto& rowBombMove3 = TutorialUtils::CreateTransparentRowBomb(*this, {1.0f, -4.0f, 0.0f}, container, pieceResources);
    auto& rowBombMove4 = TutorialUtils::CreateTransparentRowBomb(*this, {3.0f, -4.0f, 0.0f}, container, pieceResources);
    
    std::vector<Pht::Keyframe> rowBombMoveKeyframes {
        {.mTime = 0.0f, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = clickMoveTime, .mRotation = Pht::Vec3{0.0f, clickMoveTime * rowBombRotationSpeed, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(rowBombMove1, rowBombMoveKeyframes);
    animationSystem.CreateAnimation(rowBombMove2, rowBombMoveKeyframes);
    animationSystem.CreateAnimation(rowBombMove3, rowBombMoveKeyframes);
    animationSystem.CreateAnimation(rowBombMove4, rowBombMoveKeyframes);
    
    Pht::Vec3 rowBombInitialPosition {0.0f, 3.8f, 0.0f};
    Pht::Vec3 rowBombPosition2 {-2.0f, 3.8f, 0.0f};
    Pht::Vec3 rowBombDetonationPosition {-2.0f, -2.0f, 0.0f};
    
    auto& rowBomb = TutorialUtils::CreateRowBomb(*this, rowBombInitialPosition, container, pieceResources);
    
    std::vector<Pht::Keyframe> rowBombKeyframes {
        {.mTime = 0.0f, .mPosition = rowBombInitialPosition, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = clickMoveTime, .mPosition = rowBombInitialPosition, .mRotation = Pht::Vec3{0.0f, clickMoveTime * rowBombRotationSpeed, 0.0f}},
        {.mTime = rowBombPosition2Time, .mPosition = rowBombPosition2, .mRotation = Pht::Vec3{0.0f, rowBombPosition2Time * rowBombRotationSpeed, 0.0f}},
        {.mTime = detonationTime, .mPosition = rowBombDetonationPosition, .mRotation = Pht::Vec3{0.0f, detonationTime * rowBombRotationSpeed, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(rowBomb, rowBombKeyframes);
}

void LaserDialogView::SetUp() {
    if (mGuiLightProvider) {
        TutorialUtils::SetGuiLightDirections(*mGuiLightProvider);
    }
    
    mAnimation->Stop();
    mAnimation->Play();
}

void LaserDialogView::OnDeactivate() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

void LaserDialogView::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    mAnimation->Update(dt);
}
