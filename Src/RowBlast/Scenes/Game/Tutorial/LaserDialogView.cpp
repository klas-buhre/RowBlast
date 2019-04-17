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
    constexpr auto animationDuration = 4.25f;
    constexpr auto clickMoveTime = 1.5f;
    constexpr auto rowBombPosition2Time = 1.65f;
    constexpr auto detonationTime = 1.95f;
    constexpr auto blockFlyDuration = 0.5f;
    constexpr auto blockFallWaitDuration = 0.5f;
    constexpr auto blockFallDuration = 0.5f;
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
    CreateText({-3.75f, -5.5f, UiLayer::text}, "The laser clears a row", textProperties);
    
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
    container.GetTransform().SetPosition({0.0f, 1.3f, 0.0f});
    container.GetTransform().SetScale(1.25f);
    GetRoot().AddChild(container);
    
    auto& animationSystem = mEngine.GetAnimationSystem();
    mAnimation = &animationSystem.CreateAnimation(container, {{.mTime = 0.0f}, {.mTime = animationDuration}});

    TutorialUtils::CreateFieldQuad(mEngine, *this, container);
    TutorialUtils::CreateFieldQuadFrame(mEngine, *this, container);

    auto& coloredBlocks = CreateSceneObject();
    coloredBlocks.GetTransform().SetPosition({2.0f, 0.0f, 0.0f});
    container.AddChild(coloredBlocks);

    TutorialUtils::CreateColoredBlock(*this, {1.0f, 1.0f, UiLayer::block}, BlockColor::Green, coloredBlocks, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {-1.0f, -0.5f, UiLayer::block}, BlockColor::Blue, 90.0f, coloredBlocks, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {0.0f, -0.5f, UiLayer::block}, BlockColor::Yellow, 90.0f, coloredBlocks, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {1.0f, -0.5f, UiLayer::block}, BlockColor::Red, 90.0f, coloredBlocks, pieceResources);
    
    auto& grayBlock1 = TutorialUtils::CreateGrayBlock(*this, {-3.0f, -2.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock2 = TutorialUtils::CreateGrayBlock(*this, {1.0f, -2.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock3 = TutorialUtils::CreateGrayBlock(*this, {2.0f, -2.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock4 = TutorialUtils::CreateGrayBlock(*this, {3.0f, -2.0f, UiLayer::block}, container, levelResources);
    TutorialUtils::CreateThreeGrayBlocks(*this, {-2.0f, -3.0f, UiLayer::block}, container, levelResources);
    TutorialUtils::CreateThreeGrayBlocks(*this, {-2.0f, -4.0f, UiLayer::block}, container, levelResources);
    
    auto& rowBombMove1 = TutorialUtils::CreateTransparentRowBomb(*this, {-2.0f, -2.0f, UiLayer::block}, container, pieceResources);
    auto& rowBombMove2 = TutorialUtils::CreateTransparentRowBomb(*this, {2.0f, 1.0f, UiLayer::block}, container, pieceResources);
    auto& rowBombMove3 = TutorialUtils::CreateTransparentRowBomb(*this, {1.0f, -4.0f, UiLayer::block}, container, pieceResources);
    auto& rowBombMove4 = TutorialUtils::CreateTransparentRowBomb(*this, {3.0f, -4.0f, UiLayer::block}, container, pieceResources);
    
    std::vector<Pht::Keyframe> rowBombMoveKeyframes {
        {.mTime = 0.0f, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = clickMoveTime, .mRotation = Pht::Vec3{0.0f, clickMoveTime * rowBombRotationSpeed, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(rowBombMove1, rowBombMoveKeyframes);
    animationSystem.CreateAnimation(rowBombMove2, rowBombMoveKeyframes);
    animationSystem.CreateAnimation(rowBombMove3, rowBombMoveKeyframes);
    animationSystem.CreateAnimation(rowBombMove4, rowBombMoveKeyframes);
    
    Pht::Vec3 rowBombInitialPosition {0.0f, 3.8f, UiLayer::block};
    Pht::Vec3 rowBombPosition2 {-2.0f, 3.8f, UiLayer::block};
    Pht::Vec3 rowBombDetonationPosition {-2.0f, -2.0f, UiLayer::block};
    
    auto& rowBomb = TutorialUtils::CreateRowBomb(*this, rowBombInitialPosition, container, pieceResources);
    
    std::vector<Pht::Keyframe> rowBombKeyframes {
        {.mTime = 0.0f, .mPosition = rowBombInitialPosition, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = clickMoveTime, .mPosition = rowBombInitialPosition, .mRotation = Pht::Vec3{0.0f, clickMoveTime * rowBombRotationSpeed, 0.0f}},
        {.mTime = rowBombPosition2Time, .mPosition = rowBombPosition2, .mRotation = Pht::Vec3{0.0f, rowBombPosition2Time * rowBombRotationSpeed, 0.0f}},
        {.mTime = detonationTime, .mPosition = rowBombDetonationPosition, .mRotation = Pht::Vec3{0.0f, detonationTime * rowBombRotationSpeed, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(rowBomb, rowBombKeyframes);
    
    mHandAnimation = std::make_unique<HandAnimation>(mEngine, 1.0f, true);
    mHandAnimation->Init(container);
    
    std::vector<Pht::Keyframe> handAnimationClickKeyframes {
        {
            .mTime = 0.0f,
            .mCallback = [this] () {
                mHandAnimation->StartInNotTouchingScreenState({-1.5f, -2.6f, UiLayer::root},
                                                              30.0f,
                                                              10.0f);
            }
        },
        {
            .mTime = clickMoveTime - 0.3f,
            .mCallback = [this] () {
                mHandAnimation->BeginTouch(0.0f);
            }
        },
        {
            .mTime = animationDuration
        }
    };
    mHandPhtAnimation = &animationSystem.CreateAnimation(mHandAnimation->GetSceneObject(), handAnimationClickKeyframes);
    
    Pht::Vec3 handInitialPosition {2.25f, 1.0f, UiLayer::root};
    Pht::Vec3 handAfterSwipePosition {2.25f, -1.3f, UiLayer::root};

    std::vector<Pht::Keyframe> handAnimationSwipeKeyframes {
        {
            .mTime = 0.0f,
            .mPosition = handInitialPosition,
            .mCallback = [this, &handInitialPosition] () {
                mHandAnimation->StartInNotTouchingScreenState(handInitialPosition, 45.0f, 10.0f);
            }
        },
        {
            .mTime = 0.4f,
            .mPosition = handInitialPosition,
            .mCallback = [this] () {
                mHandAnimation->BeginTouch(0.45f);
            }
        },
        {
            .mTime = 0.6f,
            .mPosition = handInitialPosition
        },
        {
            .mTime = 1.0f,
            .mPosition = handAfterSwipePosition
        },
        {
            .mTime = 1.5f,
            .mPosition = handAfterSwipePosition
        },
        {
            .mTime = 3.0f,
            .mPosition = handInitialPosition
        },
        {
            .mTime = animationDuration
        }
    };
    Pht::AnimationClip handAnimationSwipeClip {handAnimationSwipeKeyframes};
    handAnimationSwipeClip.SetInterpolation(Pht::Interpolation::Cosine);
    mHandPhtAnimation->AddClip(handAnimationSwipeClip, 1);

    mLaserEffect = std::make_unique<TutorialLaserParticleEffect>(mEngine, container);
    
    std::vector<Pht::Keyframe> laserKeyframes {
        {
            .mTime = 0.0f
        },
        {
            .mTime = detonationTime,
            .mCallback = [this] () {
                mLaserEffect->StartLaser();
            }
        },
        {
            .mTime = animationDuration
        }
    };
    animationSystem.CreateAnimation(mLaserEffect->GetSceneObject(), laserKeyframes);
    
    std::vector<Pht::Keyframe> grayBlock1Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-3.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{-3.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-3.0f, -5.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock1, grayBlock1Keyframes);
    
    std::vector<Pht::Keyframe> grayBlock2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{1.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{1.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{1.0f, -5.0f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock2, grayBlock2Keyframes);

    std::vector<Pht::Keyframe> grayBlock3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{2.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{2.5f, -4.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock3, grayBlock3Keyframes);

    std::vector<Pht::Keyframe> grayBlock4Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{3.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{3.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{4.0f, -4.5f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock4, grayBlock4Keyframes);
    
    std::vector<Pht::Keyframe> coloredBlocksKeyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFallWaitDuration, .mPosition = Pht::Vec3{2.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFallWaitDuration + blockFallDuration, .mPosition = Pht::Vec3{2.0f, -3.0f, 0.0f}},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(coloredBlocks, coloredBlocksKeyframes);
}

void LaserDialogView::SetUp() {
    switch (mUserServices.GetSettingsService().GetControlType()) {
        case ControlType::Click:
            mHandPhtAnimation->SetActiveClip(0);
            break;
        case ControlType::Gesture:
            mHandPhtAnimation->SetActiveClip(1);
            break;
    }

    if (mGuiLightProvider) {
        TutorialUtils::SetGuiLightDirections(*mGuiLightProvider);
    }
    
    mLaserEffect->SetUp();
    
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
    mLaserEffect->Update(dt);
    mHandAnimation->Update();
}
