#include "AsteroidDialogView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "IAnimationSystem.hpp"
#include "Animation.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UiLayer.hpp"
#include "TutorialUiLayer.hpp"
#include "PieceResources.hpp"
#include "LevelResources.hpp"
#include "UserServices.hpp"
#include "GuiUtils.hpp"
#include "TutorialUtils.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration = 4.5f;
    constexpr auto clickMoveTime = 1.5f;
    constexpr auto piecePosition2Time = 1.65f;
    constexpr auto pieceLandTime = 1.95f;
    constexpr auto beginSwipeRightTime = 1.0f;
    constexpr auto beginSwipeDownTime = 1.5f;
    constexpr auto blockFlyDuration = 0.5f;
    constexpr auto blockFallWaitDuration = 0.65f;
    constexpr auto blockFallDuration = 0.4f;
    constexpr auto asteroidScaleUpDuration = 0.4f;
}

AsteroidDialogView::AsteroidDialogView(Pht::IEngine& engine,
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
    CreateText({-2.2f, 8.45f, UiLayer::text}, "ASTEROID", largeTextProperties);

    GuiUtils::CreateTitleBarLine(engine, *this, 2.2f);
    
    CreateAnimation(pieceResources, levelResources);

    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-4.4f, -4.8f, UiLayer::panel}, "Bring the asteroid down to", textProperties);
    CreateText({-3.8f, -5.875f, UiLayer::panel}, "the bottom of the field", textProperties);
    
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

void AsteroidDialogView::CreateAnimation(const PieceResources& pieceResources,
                                         const LevelResources& levelResources) {
    auto& container = CreateSceneObject();
    container.GetTransform().SetPosition({0.0f, 1.8f, 0.0f});
    container.GetTransform().SetScale(1.25f);
    GetRoot().AddChild(container);
    
    auto& animationSystem = mEngine.GetAnimationSystem();
    mAnimation = &animationSystem.CreateAnimation(container, {{.mTime = 0.0f}, {.mTime = animationDuration}});

    TutorialUtils::CreateFieldQuad(mEngine, *this, container);
    TutorialUtils::CreateFieldQuadFrame(mEngine, *this, container);
    
    auto& fallingCells = CreateSceneObject();
    container.AddChild(fallingCells);
    
    TutorialUtils::CreateTwoBlocks(*this, {0.5f, -1.0f, UiLayer::block}, BlockColor::Red, 0.0f, fallingCells, pieceResources);
    
    auto& asteroid = TutorialUtils::CreateAsteroid(*this, {3.0f, -1.0f, UiLayer::block}, fallingCells, levelResources);
    
    std::vector<Pht::Keyframe> asteroidKeyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{3.0f, -1.0f, UiLayer::block}, .mRotation = Pht::Vec3{-25.0f, 64.5f, -12.0f}, .mScale = Pht::Vec3{0.8f, 0.8f, 0.8f}},
        {.mTime = animationDuration / 2.0f, .mRotation = Pht::Vec3{-25.0f, 25.5f, -12.0f}},
        {.mTime = animationDuration - asteroidScaleUpDuration, .mPosition = Pht::Vec3{3.0f, -1.0f, 0.0f}, .mScale = Pht::Vec3{0.8f, 0.8f, 0.8f}},
        {.mTime = animationDuration, .mPosition = Pht::Vec3{3.0f, -1.0f, 0.0f}, .mScale = Pht::Vec3{1.5f, 1.5f, 1.5f}}
    };
    auto& asteroidAnimation = animationSystem.CreateAnimation(asteroid, asteroidKeyframes);
    asteroidAnimation.SetInterpolation(Pht::Interpolation::Cosine);
    
    auto& blockN3N2 = TutorialUtils::CreateColoredBlock(*this, {-3.0f, -2.0f, UiLayer::block}, BlockColor::Yellow, container, pieceResources);
    auto& blockN3N3 = TutorialUtils::CreateColoredBlock(*this, {-3.0f, -3.0f, UiLayer::block}, BlockColor::Red, container, pieceResources);
    auto& blockN3N4 = TutorialUtils::CreateColoredBlock(*this, {-3.0f, -4.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& blockN2N4 = TutorialUtils::CreateColoredBlock(*this, {-2.0f, -4.0f, UiLayer::block}, BlockColor::Green, container, pieceResources);

    auto& clearedCells = CreateSceneObject();
    container.AddChild(clearedCells);
    TutorialUtils::CreateTwoBlocks(*this, {0.5f, -2.0f, UiLayer::block}, BlockColor::Blue, 0.0f, clearedCells, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {2.5f, -2.0f, UiLayer::block}, BlockColor::Yellow, 0.0f, clearedCells, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {0.5f, -3.0f, UiLayer::block}, BlockColor::Yellow, 0.0f, clearedCells, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {2.5f, -3.0f, UiLayer::block}, BlockColor::Blue, 0.0f, clearedCells, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {0.5f, -4.0f, UiLayer::block}, BlockColor::Green, 0.0f, clearedCells, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {2.5f, -4.0f, UiLayer::block}, BlockColor::Red, 0.0f, clearedCells, pieceResources);
    
    std::vector<Pht::Keyframe> clearedCellsKeyframes {
        {.mTime = 0.0f, .mIsVisible = true},
        {.mTime = pieceLandTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(clearedCells, clearedCellsKeyframes);
    
    mBottomGlowAnimation = std::make_unique<TutorialFieldBottomGlowAnimation>(mEngine, container);

    mMoves = &CreateSceneObject();
    container.AddChild(*mMoves);
    auto& move1 = TutorialUtils::CreatePieceGhostPiece(*this, "B", {-3.0f, 0.0f, UiLayer::block}, -90.0f, *mMoves, levelResources);
    auto& move2 = TutorialUtils::CreatePieceGhostPiece(*this, "B", {-1.0f, -3.0f, UiLayer::block}, 90.0f, *mMoves, levelResources);
    auto& move3 = TutorialUtils::CreatePieceGhostPiece(*this, "B", {2.0f, 0.0f, UiLayer::block}, 90.0f, *mMoves, levelResources);
    
    std::vector<Pht::Keyframe> moveKeyframes {
        {.mTime = 0.0f, .mIsVisible = true},
        {.mTime = clickMoveTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(move1, moveKeyframes);
    animationSystem.CreateAnimation(move2, moveKeyframes);
    animationSystem.CreateAnimation(move3, moveKeyframes);
    
    Pht::Vec3 ghostPieceInitialPosition {-0.5f, 1.5f, UiLayer::block};
    Pht::Vec3 ghostPiecePosition2 {-1.5f, 0.5f, UiLayer::block};

    mGhostPieceContainer = &CreateSceneObject();
    container.AddChild(*mGhostPieceContainer);
    auto& ghostPiece = TutorialUtils::CreatePieceGhostPiece(*this, "B", ghostPieceInitialPosition, 0.0f, *mGhostPieceContainer, levelResources);

    std::vector<Pht::Keyframe> ghostPieceKeyframes {
        {.mTime = 0.0f, .mPosition = ghostPieceInitialPosition, .mIsVisible = true},
        {.mTime = beginSwipeRightTime, .mPosition = ghostPieceInitialPosition},
        {.mTime = beginSwipeDownTime, .mPosition = ghostPiecePosition2},
        {.mTime = pieceLandTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    auto& ghostPieceAnimation = animationSystem.CreateAnimation(ghostPiece, ghostPieceKeyframes);
    ghostPieceAnimation.SetInterpolation(Pht::Interpolation::None);

    Pht::Vec3 pieceInitialPosition {-0.5f, 2.8f, UiLayer::block};
    Pht::Vec3 piecePosition2 {-1.5f, 2.8f, UiLayer::block};
    Pht::Vec3 pieceLandingPosition {-1.5f, -3.0f, UiLayer::block};
    
    auto& piece = TutorialUtils::CreateBPiece(*this, pieceInitialPosition, 90.0f, container, pieceResources);
    
    std::vector<Pht::Keyframe> pieceKeyframes {
        {.mTime = 0.0f, .mPosition = pieceInitialPosition, .mIsVisible = true},
        {.mTime = clickMoveTime, .mPosition = pieceInitialPosition},
        {.mTime = piecePosition2Time, .mPosition = piecePosition2},
        {.mTime = pieceLandTime, .mPosition = pieceLandingPosition, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    mPieceAnimation = &animationSystem.CreateAnimation(piece, pieceKeyframes);
    
    std::vector<Pht::Keyframe> pieceSwipeKeyframes {
        {.mTime = 0.0f, .mPosition = pieceInitialPosition, .mIsVisible = true},
        {.mTime = beginSwipeRightTime, .mPosition = pieceInitialPosition},
        {.mTime = beginSwipeDownTime, .mPosition = piecePosition2},
        {.mTime = pieceLandTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    Pht::AnimationClip pieceSwipeClip {pieceSwipeKeyframes};
    pieceSwipeClip.SetInterpolation(Pht::Interpolation::None);
    mPieceAnimation->AddClip(pieceSwipeClip, 1);
    
    mHandAnimation = std::make_unique<HandAnimation>(mEngine, 1.0f, true);
    mHandAnimation->Init(container);
    
    std::vector<Pht::Keyframe> handAnimationClickKeyframes {
        {
            .mTime = 0.0f,
            .mCallback = [this] () {
                mHandAnimation->StartInNotTouchingScreenState({-2.5f, -2.5f, UiLayer::root},
                                                              -120.0f,
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
    
    Pht::Vec3 swipePos {1.0f, -2.0f, 0.0f};
    auto handInitialPosition = swipePos + Pht::Vec3{2.25f, 1.0f, UiLayer::root};
    auto handAfterSwipeLeftPosition = swipePos + Pht::Vec3{1.25f, 1.0f, UiLayer::root};
    auto handAfterSwipeDownPosition = swipePos + Pht::Vec3{1.25f, -1.3f, UiLayer::root};

    std::vector<Pht::Keyframe> handAnimationSwipeKeyframes {
        {
            .mTime = 0.0f,
            .mPosition = handInitialPosition,
            .mCallback = [this, &handInitialPosition] () {
                mHandAnimation->StartInNotTouchingScreenState(handInitialPosition, 45.0f, 10.0f);
            }
        },
        {
            .mTime = beginSwipeRightTime - 0.2f,
            .mPosition = handInitialPosition,
            .mCallback = [this] () {
                mHandAnimation->BeginTouch(pieceLandTime - beginSwipeRightTime);
            }
        },
        {
            .mTime = beginSwipeRightTime,
            .mPosition = handInitialPosition
        },
        {
            .mTime = beginSwipeDownTime,
            .mPosition = handAfterSwipeLeftPosition
        },
        {
            .mTime = pieceLandTime - 0.1f,
            .mPosition = handAfterSwipeDownPosition
        },
        {
            .mTime = pieceLandTime + 0.5f,
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
    Pht::AnimationClip handAnimationSwipeClip {handAnimationSwipeKeyframes};
    handAnimationSwipeClip.SetInterpolation(Pht::Interpolation::Cosine);
    mHandPhtAnimation->AddClip(handAnimationSwipeClip, 1);

    std::vector<Pht::Keyframe> blockN3N2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-3.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{-3.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{-3.9f, -4.5f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blockN3N2, blockN3N2Keyframes);

    std::vector<Pht::Keyframe> blockN3N3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-3.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{-3.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{-4.2f, -6.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blockN3N3, blockN3N3Keyframes);

    std::vector<Pht::Keyframe> blockN3N4Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-3.0f, -4.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{-3.0f, -4.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{-4.0f, -7.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blockN3N4, blockN3N4Keyframes);
    
    auto& blockN2N2 = TutorialUtils::CreateColoredBlock(*this, {-2.0f, -2.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& blockN2N3 = TutorialUtils::CreateColoredBlock(*this, {-2.0f, -3.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& blockN1N2 = TutorialUtils::CreateColoredBlock(*this, {-1.0f, -2.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& blockN1N3 = TutorialUtils::CreateColoredBlock(*this, {-1.0f, -3.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& blockN1N4 = TutorialUtils::CreateColoredBlock(*this, {-1.0f, -4.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& block0N2 = TutorialUtils::CreateColoredBlock(*this, {0.0f, -2.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& block0N3 = TutorialUtils::CreateColoredBlock(*this, {0.0f, -3.0f, UiLayer::block}, BlockColor::Yellow, container, pieceResources);
    auto& block0N4 = TutorialUtils::CreateColoredBlock(*this, {0.0f, -4.0f, UiLayer::block}, BlockColor::Green, container, pieceResources);
    auto& block1N2 = TutorialUtils::CreateColoredBlock(*this, {1.0f, -2.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& block1N3 = TutorialUtils::CreateColoredBlock(*this, {1.0f, -3.0f, UiLayer::block}, BlockColor::Yellow, container, pieceResources);
    auto& block1N4 = TutorialUtils::CreateColoredBlock(*this, {1.0f, -4.0f, UiLayer::block}, BlockColor::Green, container, pieceResources);
    auto& block2N2 = TutorialUtils::CreateColoredBlock(*this, {2.0f, -2.0f, UiLayer::block}, BlockColor::Yellow, container, pieceResources);
    auto& block2N3 = TutorialUtils::CreateColoredBlock(*this, {2.0f, -3.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& block2N4 = TutorialUtils::CreateColoredBlock(*this, {2.0f, -4.0f, UiLayer::block}, BlockColor::Red, container, pieceResources);
    auto& block3N2 = TutorialUtils::CreateColoredBlock(*this, {3.0f, -2.0f, UiLayer::block}, BlockColor::Yellow, container, pieceResources);
    auto& block3N3 = TutorialUtils::CreateColoredBlock(*this, {3.0f, -3.0f, UiLayer::block}, BlockColor::Blue, container, pieceResources);
    auto& block3N4 = TutorialUtils::CreateColoredBlock(*this, {3.0f, -4.0f, UiLayer::block}, BlockColor::Red, container, pieceResources);

    std::vector<Pht::Keyframe> blockN2N2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-2.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{-2.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{-2.5f, -5.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blockN2N2, blockN2N2Keyframes);

    std::vector<Pht::Keyframe> blockN2N3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-2.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{-2.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{-2.5f, -6.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blockN2N3, blockN2N3Keyframes);

    std::vector<Pht::Keyframe> blockN2N4Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-2.0f, -4.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{-2.0f, -4.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{-2.5f, -7.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 150.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blockN2N4, blockN2N4Keyframes);

    std::vector<Pht::Keyframe> blockN1N2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-1.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{-1.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{-1.0f, -4.7f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 50.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blockN1N2, blockN1N2Keyframes);

    std::vector<Pht::Keyframe> blockN1N3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-1.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{-1.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{-1.0f, -6.1f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blockN1N3, blockN1N3Keyframes);

    std::vector<Pht::Keyframe> blockN1N4Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-1.0f, -4.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{-1.0f, -4.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{-1.0f, -7.2f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blockN1N4, blockN1N4Keyframes);

    std::vector<Pht::Keyframe> block0N2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{0.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{0.0f, -5.0f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 150.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block0N2, block0N2Keyframes);

    std::vector<Pht::Keyframe> block0N3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{0.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{0.0f, -6.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 50.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block0N3, block0N3Keyframes);

    std::vector<Pht::Keyframe> block0N4Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, -4.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{0.0f, -4.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{0.0f, -7.7f, 0.0f}, .mRotation = Pht::Vec3{70.0f, 50.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block0N4, block0N4Keyframes);

    std::vector<Pht::Keyframe> block1N2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{1.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{1.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{1.0f, -4.4f, 0.0f}, .mRotation = Pht::Vec3{150.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block1N2, block1N2Keyframes);

    std::vector<Pht::Keyframe> block1N3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{1.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{1.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{1.0f, -6.2f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 150.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block1N3, block1N3Keyframes);

    std::vector<Pht::Keyframe> block1N4Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{1.0f, -4.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{1.0f, -4.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{1.0f, -7.3f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 150.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block1N4, block1N4Keyframes);

    std::vector<Pht::Keyframe> block2N2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{2.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{2.4f, -5.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 50.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block2N2, block2N2Keyframes);

    std::vector<Pht::Keyframe> block2N3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{2.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{2.5f, -6.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 100.0f, 150.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block2N3, block2N3Keyframes);

    std::vector<Pht::Keyframe> block2N4Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, -4.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{2.0f, -4.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{2.5f, -6.9f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block2N4, block2N4Keyframes);

    std::vector<Pht::Keyframe> block3N2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{3.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{3.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{3.9f, -5.0f, 0.0f}, .mRotation = Pht::Vec3{150.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block3N2, block3N2Keyframes);

    std::vector<Pht::Keyframe> block3N3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{3.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{3.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{4.0f, -6.2f, 0.0f}, .mRotation = Pht::Vec3{150.0f, 0.0f, 10.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block3N3, block3N3Keyframes);

    std::vector<Pht::Keyframe> block3N4Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{3.0f, -4.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = false},
        {.mTime = pieceLandTime, .mPosition = Pht::Vec3{3.0f, -4.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = pieceLandTime + blockFlyDuration, .mPosition = Pht::Vec3{4.2f, -7.5f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 150.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(block3N4, block3N4Keyframes);

    std::vector<Pht::Keyframe> fallingCellsKeyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = pieceLandTime + blockFallWaitDuration, .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = pieceLandTime + blockFallWaitDuration + blockFallDuration, .mPosition = Pht::Vec3{0.0f, -3.0f, 0.0f}},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(fallingCells, fallingCellsKeyframes);
}

void AsteroidDialogView::SetUp() {
    switch (mUserServices.GetSettingsService().GetControlType()) {
        case ControlType::Click:
            mHandPhtAnimation->SetActiveClip(0);
            mPieceAnimation->SetActiveClip(0);
            mMoves->SetIsVisible(true);
            mGhostPieceContainer->SetIsVisible(false);
            break;
        case ControlType::Gesture:
            mHandPhtAnimation->SetActiveClip(1);
            mPieceAnimation->SetActiveClip(1);
            mMoves->SetIsVisible(false);
            mGhostPieceContainer->SetIsVisible(true);
            break;
    }

    if (mGuiLightProvider) {
        TutorialUtils::SetGuiLightDirections(*mGuiLightProvider);
    }
    
    mAnimation->Stop();
    mAnimation->Play();
}

void AsteroidDialogView::OnDeactivate() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

void AsteroidDialogView::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    mAnimation->Update(dt);
    mBottomGlowAnimation->Update(dt);
    mHandAnimation->Update();
}
