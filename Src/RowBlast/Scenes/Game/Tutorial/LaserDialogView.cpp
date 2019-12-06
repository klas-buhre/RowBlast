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
    constexpr auto animationDuration = 4.1f;
    constexpr auto clickMoveTime = 1.5f;
    constexpr auto rowBombPosition2Time = 1.65f;
    constexpr auto detonationTime = 1.95f;
    constexpr auto beginSwipeLeftTime = 0.7f;
    constexpr auto beginSwipeDownTime = 1.5f;
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
    CreateText({-3.75f, -5.5f, UiLayer::panel}, "The laser clears a row", textProperties);
    
    mPlayButton = GuiUtils::CreateMediumPlayButton(engine,
                                                   *this,
                                                   {0.0f, -8.2f, UiLayer::textRectangle},
                                                   guiResources,
                                                   zoom);
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
    
    mRowBombMoves = &CreateSceneObject();
    container.AddChild(*mRowBombMoves);
    auto& rowBombMove1 = TutorialUtils::CreateTransparentRowBomb(*this, {-2.0f, -2.0f, UiLayer::block}, *mRowBombMoves, pieceResources);
    auto& rowBombMove2 = TutorialUtils::CreateTransparentRowBomb(*this, {2.0f, 1.0f, UiLayer::block}, *mRowBombMoves, pieceResources);
    auto& rowBombMove3 = TutorialUtils::CreateTransparentRowBomb(*this, {1.0f, -4.0f, UiLayer::block}, *mRowBombMoves, pieceResources);
    auto& rowBombMove4 = TutorialUtils::CreateTransparentRowBomb(*this, {3.0f, -4.0f, UiLayer::block}, *mRowBombMoves, pieceResources);
    
    std::vector<Pht::Keyframe> rowBombMoveKeyframes {
        {.mTime = 0.0f, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = clickMoveTime, .mRotation = Pht::Vec3{0.0f, clickMoveTime * rowBombRotationSpeed, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(rowBombMove1, rowBombMoveKeyframes);
    animationSystem.CreateAnimation(rowBombMove2, rowBombMoveKeyframes);
    animationSystem.CreateAnimation(rowBombMove3, rowBombMoveKeyframes);
    animationSystem.CreateAnimation(rowBombMove4, rowBombMoveKeyframes);
    
    Pht::Vec3 rowBombGhostPieceInitialPosition {0.0f, -4.0f, UiLayer::block};
    Pht::Vec3 rowBombGhostPiecePosition2 {-1.0f, -2.0f, UiLayer::block};
    Pht::Vec3 rowBombGhostPiecePosition3 {-2.0f, -2.0f, UiLayer::block};

    mRowBombGhostPieceContainer = &CreateSceneObject();
    container.AddChild(*mRowBombGhostPieceContainer);
    auto& rowBombGhostPiece = TutorialUtils::CreateTransparentRowBomb(*this, rowBombGhostPieceInitialPosition, *mRowBombGhostPieceContainer, pieceResources);

    std::vector<Pht::Keyframe> rowBombGhostPieceKeyframes {
        {.mTime = 0.0f, .mPosition = rowBombGhostPieceInitialPosition, .mIsVisible = true},
        {.mTime = beginSwipeLeftTime, .mPosition = rowBombGhostPieceInitialPosition},
        {.mTime = beginSwipeLeftTime + (beginSwipeDownTime - beginSwipeLeftTime) / 2.0f, .mPosition = rowBombGhostPiecePosition2},
        {.mTime = beginSwipeDownTime, .mPosition = rowBombGhostPiecePosition3},
        {.mTime = detonationTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    auto& rowBombGhostPieceAnimation = animationSystem.CreateAnimation(rowBombGhostPiece, rowBombGhostPieceKeyframes);
    rowBombGhostPieceAnimation.SetInterpolation(Pht::Interpolation::None);

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
    mRowBombAnimation = &animationSystem.CreateAnimation(rowBomb, rowBombKeyframes);
    
    std::vector<Pht::Keyframe> rowBombSwipeKeyframes {
        {.mTime = 0.0f, .mPosition = rowBombInitialPosition, .mIsVisible = true},
        {.mTime = beginSwipeLeftTime, .mPosition = rowBombInitialPosition},
        {.mTime = beginSwipeLeftTime + (beginSwipeDownTime - beginSwipeLeftTime) / 2.0f, .mPosition = Pht::Vec3{-1.0f, 3.8f, UiLayer::block}},
        {.mTime = beginSwipeDownTime, .mPosition = rowBombPosition2},
        {.mTime = detonationTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    auto& rowBombSwipeClip = mRowBombAnimation->CreateClip(rowBombSwipeKeyframes, 1);
    rowBombSwipeClip.SetInterpolation(Pht::Interpolation::None);
    
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
    
    Pht::Vec3 swipePos {0.5f, -0.5f, 0.0f};
    auto handInitialPosition = swipePos + Pht::Vec3{2.25f, 1.0f, UiLayer::root};
    auto handAfterSwipeLeftPosition = swipePos + Pht::Vec3{0.25f, 1.0f, UiLayer::root};
    auto handAfterSwipeDownPosition = swipePos + Pht::Vec3{0.25f, -2.1f, UiLayer::root};

    std::vector<Pht::Keyframe> handAnimationSwipeKeyframes {
        {
            .mTime = 0.0f,
            .mPosition = handInitialPosition,
            .mCallback = [this, &handInitialPosition] () {
                mHandAnimation->StartInNotTouchingScreenState(handInitialPosition, 45.0f, 10.0f);
            }
        },
        {
            .mTime = beginSwipeLeftTime - 0.2f,
            .mPosition = handInitialPosition,
            .mCallback = [this] () {
                mHandAnimation->BeginTouch(detonationTime - beginSwipeLeftTime);
            }
        },
        {
            .mTime = beginSwipeLeftTime,
            .mPosition = handInitialPosition
        },
        {
            .mTime = beginSwipeDownTime,
            .mPosition = handAfterSwipeLeftPosition
        },
        {
            .mTime = detonationTime - 0.1f,
            .mPosition = handAfterSwipeDownPosition
        },
        {
            .mTime = detonationTime + 0.5f,
            .mPosition = handAfterSwipeDownPosition
        },
        {
            .mTime = animationDuration - 0.25f,
            .mPosition = handInitialPosition
        },
        {
            .mTime = animationDuration
        }
    };
    auto& handAnimationSwipeClip = mHandPhtAnimation->CreateClip(handAnimationSwipeKeyframes, 1);
    handAnimationSwipeClip.SetInterpolation(Pht::Interpolation::Cosine);

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
            mHandPhtAnimation->SetDefaultClip(0);
            mRowBombAnimation->SetDefaultClip(0);
            mRowBombMoves->SetIsVisible(true);
            mRowBombGhostPieceContainer->SetIsVisible(false);
            break;
        case ControlType::Drag:
        case ControlType::Swipe:
            mHandPhtAnimation->SetDefaultClip(1);
            mRowBombAnimation->SetDefaultClip(1);
            mRowBombMoves->SetIsVisible(false);
            mRowBombGhostPieceContainer->SetIsVisible(true);
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
