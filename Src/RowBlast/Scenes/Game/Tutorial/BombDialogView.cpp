#include "BombDialogView.hpp"

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
#include "BlastRadiusAnimation.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration = 3.5f;
    constexpr auto clickMoveTime = 1.5f;
    constexpr auto clickMoveDuration = 0.5f;
    constexpr auto bombPosition2Time = 1.65f;
    constexpr auto detonationTime = 1.95f;
    constexpr auto beginSwipeRightTime = 1.0f;
    constexpr auto beginSwipeDownTime = 1.5f;
    constexpr auto blockFlyDuration = 0.5f;
}

BombDialogView::BombDialogView(Pht::IEngine& engine,
                               const CommonResources& commonResources,
                               const PieceResources& pieceResources,
                               const LevelResources& levelResources,
                               const BlastRadiusAnimation& blastRadiusAnimation,
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
    CreateText({-1.4f, 8.25f, UiLayer::text}, "BOMB", largeTextProperties);

    GuiUtils::CreateTitleBarLine(engine, *this);
    
    CreateAnimation(pieceResources, levelResources, blastRadiusAnimation);
    
    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-4.15f, -5.5f, UiLayer::panel}, "The bomb clears an area", textProperties);
    
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

void BombDialogView::CreateAnimation(const PieceResources& pieceResources,
                                     const LevelResources& levelResources,
                                     const BlastRadiusAnimation& blastRadiusAnimation) {
    auto& container = CreateSceneObject();
    container.GetTransform().SetPosition({0.0f, 1.3f, 0.0f});
    container.GetTransform().SetScale(1.25f);
    GetRoot().AddChild(container);
    
    auto& animationSystem = mEngine.GetAnimationSystem();
    mAnimation = &animationSystem.CreateAnimation(container, {{.mTime = 0.0f}, {.mTime = animationDuration}});

    TutorialUtils::CreateFieldQuad(mEngine, *this, container);
    TutorialUtils::CreateFieldQuadFrame(mEngine, *this, container);

    auto& coloredPieces = CreateSceneObject();
    container.AddChild(coloredPieces);

    TutorialUtils::CreateTwoBlocks(*this, {-1.0f, -0.5f, UiLayer::block}, BlockColor::Blue, 90.0f, coloredPieces, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {0.0f, -0.5f, UiLayer::block}, BlockColor::Yellow, 90.0f, coloredPieces, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {2.0f, -0.5f, UiLayer::block}, BlockColor::Red, 90.0f, coloredPieces, pieceResources);
    TutorialUtils::CreateTwoBlocks(*this, {3.0f, -0.5f, UiLayer::block}, BlockColor::Green, 90.0f, coloredPieces, pieceResources);
    
    std::vector<Pht::Keyframe> coloredPiecesKeyframes {
        {.mTime = 0.0f, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(coloredPieces, coloredPiecesKeyframes);

    auto& coloredBlocks = CreateSceneObject();
    container.AddChild(coloredBlocks);
    
    std::vector<Pht::Keyframe> coloredBlocksKeyframes {
        {.mTime = 0.0f, .mIsVisible = false},
        {.mTime = detonationTime + 0.1f, .mIsVisible = true},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(coloredBlocks, coloredBlocksKeyframes);

    auto& blueBlock1 = TutorialUtils::CreateColoredBlock(*this, {-1.0f, 0.0f, UiLayer::block}, BlockColor::Blue, coloredBlocks, pieceResources);
    auto& blueBlock2 = TutorialUtils::CreateColoredBlock(*this, {-1.0f, -1.0f, UiLayer::block}, BlockColor::Blue, coloredBlocks, pieceResources);
    auto& yellowBlock1 = TutorialUtils::CreateColoredBlock(*this, {0.0f, 0.0f, UiLayer::block}, BlockColor::Yellow, coloredBlocks, pieceResources);
    auto& yellowBlock2 = TutorialUtils::CreateColoredBlock(*this, {0.0f, -1.0f, UiLayer::block}, BlockColor::Yellow, coloredBlocks, pieceResources);
    auto& redBlock1 = TutorialUtils::CreateColoredBlock(*this, {2.0f, 0.0f, UiLayer::block}, BlockColor::Red, coloredBlocks, pieceResources);
    auto& redBlock2 = TutorialUtils::CreateColoredBlock(*this, {2.0f, -1.0f, UiLayer::block}, BlockColor::Red, coloredBlocks, pieceResources);
    auto& greenBlock1 = TutorialUtils::CreateColoredBlock(*this, {3.0f, 0.0f, UiLayer::block}, BlockColor::Green, coloredBlocks, pieceResources);
    auto& greenBlock2 = TutorialUtils::CreateColoredBlock(*this, {3.0f, -1.0f, UiLayer::block}, BlockColor::Green, coloredBlocks, pieceResources);

    std::vector<Pht::Keyframe> blueBlock1Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-1.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{-1.0f, 0.0f, TutorialUiLayer::root}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-3.0f, 1.0f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blueBlock1, blueBlock1Keyframes);
    
    std::vector<Pht::Keyframe> blueBlock2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-1.0f, -1.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{-1.0f, -1.0f, TutorialUiLayer::root}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-4.0f, -1.0f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(blueBlock2, blueBlock2Keyframes);

    std::vector<Pht::Keyframe> yellowBlock1Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{0.0f, 0.0f, TutorialUiLayer::root}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-1.5f, 1.25f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(yellowBlock1, yellowBlock1Keyframes);
    
    std::vector<Pht::Keyframe> yellowBlock2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, -1.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{0.0f, -1.0f, TutorialUiLayer::root}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-2.5f, -1.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(yellowBlock2, yellowBlock2Keyframes);

    std::vector<Pht::Keyframe> redBlock1Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{2.0f, 0.0f, TutorialUiLayer::root}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{3.5f, 1.25f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(redBlock1, redBlock1Keyframes);
    
    std::vector<Pht::Keyframe> redBlock2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, -1.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{2.0f, -1.0f, TutorialUiLayer::root}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{4.5f, -1.0f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(redBlock2, redBlock2Keyframes);

    std::vector<Pht::Keyframe> greenBlock1Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{3.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{3.0f, 0.0f, TutorialUiLayer::root}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{5.0f, 1.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(greenBlock1, greenBlock1Keyframes);
    
    std::vector<Pht::Keyframe> greenBlock2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{3.0f, -1.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{3.0f, -1.0f, TutorialUiLayer::root}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{6.0f, -1.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(greenBlock2, greenBlock2Keyframes);

    auto& grayBlock0 = TutorialUtils::CreateGrayBlock(*this, {-1.0f, -2.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock1 = TutorialUtils::CreateGrayBlock(*this, {0.0f, -2.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock2 = TutorialUtils::CreateGrayBlock(*this, {1.0f, -2.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock3 = TutorialUtils::CreateGrayBlock(*this, {2.0f, -2.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock4 = TutorialUtils::CreateGrayBlock(*this, {3.0f, -2.0f, UiLayer::block}, container, levelResources);

    TutorialUtils::CreateGrayBlock(*this, {-1.0f, -3.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock5 = TutorialUtils::CreateGrayBlock(*this, {0.0f, -3.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock6 = TutorialUtils::CreateGrayBlock(*this, {1.0f, -3.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock7 = TutorialUtils::CreateGrayBlock(*this, {2.0f, -3.0f, UiLayer::block}, container, levelResources);
    TutorialUtils::CreateGrayBlock(*this, {3.0f, -3.0f, UiLayer::block}, container, levelResources);

    TutorialUtils::CreateThreeGrayBlocks(*this, {0.0f, -4.0f, UiLayer::block}, container, levelResources);
    TutorialUtils::CreateGrayBlock(*this, {2.0f, -4.0f, UiLayer::block}, container, levelResources);
    TutorialUtils::CreateGrayBlock(*this, {3.0f, -4.0f, UiLayer::block}, container, levelResources);
    
    mBombMoves = &CreateSceneObject();
    container.AddChild(*mBombMoves);
    auto& bombMove1 = TutorialUtils::CreateTransparentBomb(*this, {-2.0f, -4.0f, UiLayer::block}, *mBombMoves, pieceResources);
    auto& bombMove2 = TutorialUtils::CreateTransparentBomb(*this, {-1.0f, 1.0f, UiLayer::block}, *mBombMoves, pieceResources);
    auto& bombMove3 = TutorialUtils::CreateTransparentBomb(*this, {1.0f, -1.0f, UiLayer::block}, *mBombMoves, pieceResources);
    auto& bombMove4 = TutorialUtils::CreateTransparentBomb(*this, {3.0f, 1.0f, UiLayer::block}, *mBombMoves, pieceResources);
    
    std::vector<Pht::Keyframe> bombMoveKeyframes {
        {.mTime = 0.0f, .mRotation = Pht::Vec3{65.0f, -22.0f, 0.0f}, .mIsVisible = true},
        {.mTime = clickMoveTime - clickMoveDuration, .mRotation = Pht::Vec3{90.0f, -32.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(bombMove1, bombMoveKeyframes);
    animationSystem.CreateAnimation(bombMove2, bombMoveKeyframes);
    animationSystem.CreateAnimation(bombMove4, bombMoveKeyframes);
    
    std::vector<Pht::Keyframe> bombMove3Keyframes {
        {.mTime = 0.0f, .mRotation = Pht::Vec3{65.0f, -22.0f, 0.0f}, .mIsVisible = true},
        {.mTime = clickMoveTime, .mRotation = Pht::Vec3{90.0f, -32.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(bombMove3, bombMove3Keyframes);
    
    Pht::Vec3 bombGhostPieceInitialPosition {0.0f, 1.0f, UiLayer::block};
    Pht::Vec3 bombGhostPiecePosition2 {1.0f, -1.0f, UiLayer::block};

    mBombGhostPieceContainer = &CreateSceneObject();
    container.AddChild(*mBombGhostPieceContainer);
    auto& bombGhostPiece = TutorialUtils::CreateTransparentBomb(*this, bombGhostPieceInitialPosition, *mBombGhostPieceContainer, pieceResources);

    std::vector<Pht::Keyframe> bombGhostPieceKeyframes {
        {.mTime = 0.0f, .mPosition = bombGhostPieceInitialPosition, .mRotation = Pht::Vec3{75.0f, -27.0f, 0.0f}, .mIsVisible = true},
        {.mTime = beginSwipeRightTime, .mPosition = bombGhostPieceInitialPosition},
        {.mTime = beginSwipeDownTime, .mPosition = bombGhostPiecePosition2},
        {.mTime = detonationTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    auto& bombGhostPieceAnimation = animationSystem.CreateAnimation(bombGhostPiece, bombGhostPieceKeyframes);
    bombGhostPieceAnimation.SetInterpolation(Pht::Interpolation::None);

    Pht::Vec3 bombInitialPosition {0.0f, 3.8f, UiLayer::block};
    Pht::Vec3 bombPosition2 {1.0f, 3.8f, UiLayer::block};
    Pht::Vec3 bombDetonationPosition {1.0f, -1.0f, UiLayer::block};
    
    auto& bomb = TutorialUtils::CreateBomb(*this, bombInitialPosition, container, pieceResources);
    
    std::vector<Pht::Keyframe> bombKeyframes {
        {.mTime = 0.0f, .mPosition = bombInitialPosition, .mRotation = Pht::Vec3{65.0f, -22.0f, 0.0f}, .mIsVisible = true},
        {.mTime = clickMoveTime, .mPosition = bombInitialPosition, .mRotation = Pht::Vec3{90.0f, -32.0f, 0.0f}},
        {.mTime = bombPosition2Time, .mPosition = bombPosition2, .mRotation = Pht::Vec3{90.0f, -32.0f, 0.0f}},
        {.mTime = detonationTime, .mPosition = bombDetonationPosition, .mRotation = Pht::Vec3{90.0f, -32.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    mBombAnimation = &animationSystem.CreateAnimation(bomb, bombKeyframes);
    
    std::vector<Pht::Keyframe> bombSwipeKeyframes {
        {.mTime = 0.0f, .mPosition = bombInitialPosition, .mRotation = Pht::Vec3{75.0f, -27.0f, 0.0f}, .mIsVisible = true},
        {.mTime = beginSwipeRightTime, .mPosition = bombInitialPosition},
        {.mTime = beginSwipeDownTime, .mPosition = bombPosition2},
        {.mTime = detonationTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    Pht::AnimationClip bombSwipeClip {bombSwipeKeyframes};
    bombSwipeClip.SetInterpolation(Pht::Interpolation::None);
    mBombAnimation->AddClip(bombSwipeClip, 1);

    mHandAnimation = std::make_unique<HandAnimation>(mEngine, 1.0f, true);
    mHandAnimation->Init(container);
    
    std::vector<Pht::Keyframe> handAnimationClickKeyframes {
        {
            .mTime = 0.0f,
            .mCallback = [this] () {
                mHandAnimation->StartInNotTouchingScreenState({0.1f, -1.2f, UiLayer::root},
                                                              -90.0f,
                                                              10.0f);
            }
        },
        {
            .mTime = clickMoveTime - 0.3f - clickMoveDuration,
            .mCallback = [this] () {
                mHandAnimation->BeginTouch(clickMoveDuration);
            }
        },
        {
            .mTime = animationDuration
        }
    };
    mHandPhtAnimation = &animationSystem.CreateAnimation(mHandAnimation->GetSceneObject(), handAnimationClickKeyframes);
    
    Pht::Vec3 swipePos {-5.0f, -2.0f, 0.0f};
    auto handInitialPosition = swipePos + Pht::Vec3{2.25f, 1.0f, UiLayer::root};
    auto handAfterSwipeRightPosition = swipePos + Pht::Vec3{3.25f, 1.0f, UiLayer::root};
    auto handAfterSwipeDownPosition = swipePos + Pht::Vec3{3.25f, -1.3f, UiLayer::root};

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
                mHandAnimation->BeginTouch(detonationTime - beginSwipeRightTime);
            }
        },
        {
            .mTime = beginSwipeRightTime,
            .mPosition = handInitialPosition
        },
        {
            .mTime = beginSwipeDownTime,
            .mPosition = handAfterSwipeRightPosition
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
    Pht::AnimationClip handAnimationSwipeClip {handAnimationSwipeKeyframes};
    handAnimationSwipeClip.SetInterpolation(Pht::Interpolation::Cosine);
    mHandPhtAnimation->AddClip(handAnimationSwipeClip, 1);
    
    mBlastRadius = &CreateSceneObject();
    mBlastRadius->GetTransform().SetScale(0.8f);
    mBlastRadius->SetRenderable(&blastRadiusAnimation.GetBombRadiusRenderable());
    container.AddChild(*mBlastRadius);
    
    std::vector<Pht::Keyframe> blastRadiusKeyframes {
        {
            .mTime = 0.0f,
            .mIsVisible = false
        },
        {
            .mTime = clickMoveTime - clickMoveDuration,
            .mPosition = Pht::Vec3{1.0f, -1.0f, TutorialUiLayer::blastRadius},
            .mIsVisible = true
        },
        {
            .mTime = clickMoveTime,
            .mIsVisible = false
        },
        {
            .mTime = animationDuration
        }
    };
    mBlastRadiusPhtAnimation = &animationSystem.CreateAnimation(*mBlastRadius, blastRadiusKeyframes);
    
    std::vector<Pht::Keyframe> blastRadiusSwipeKeyframes {
        {
            .mTime = 0.0f,
            .mPosition = Pht::Vec3{0.0f, 1.0f, TutorialUiLayer::blastRadius},
            .mIsVisible = true
        },
        {
            .mTime = beginSwipeDownTime,
            .mPosition = Pht::Vec3{1.0f, -1.0f, TutorialUiLayer::blastRadius},
        },
        {
            .mTime = detonationTime,
            .mIsVisible = false
        },
        {
            .mTime = animationDuration
        }
    };
    Pht::AnimationClip blastRadiusSwipeClip {blastRadiusSwipeKeyframes};
    blastRadiusSwipeClip.SetInterpolation(Pht::Interpolation::None);
    mBlastRadiusPhtAnimation->AddClip(blastRadiusSwipeClip, 1);
    
    mExplosionEffect = std::make_unique<TutorialExplosionParticleEffect>(mEngine, TutorialExplosionParticleEffect::Kind::Bomb, container);
    
    std::vector<Pht::Keyframe> explosionKeyframes {
        {
            .mTime = 0.0f
        },
        {
            .mTime = detonationTime,
            .mCallback = [this] () {
                mExplosionEffect->StartExplosion({1.0f, -1.0f, 0.5f});
            }
        },
        {
            .mTime = animationDuration
        }
    };
    animationSystem.CreateAnimation(mExplosionEffect->GetSceneObject(), explosionKeyframes);

    std::vector<Pht::Keyframe> grayBlock0Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-1.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{-1.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-3.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock0, grayBlock0Keyframes);

    std::vector<Pht::Keyframe> grayBlock1Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{0.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-2.0f, -5.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
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
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{4.5f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock4, grayBlock4Keyframes);
    
    std::vector<Pht::Keyframe> grayBlock5Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{0.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-1.0f, -5.5f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock5, grayBlock5Keyframes);

    std::vector<Pht::Keyframe> grayBlock6Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{1.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{1.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{1.0, -6.25f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock6, grayBlock6Keyframes);

    std::vector<Pht::Keyframe> grayBlock7Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{2.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{3.0f, -5.5f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 50.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock7, grayBlock7Keyframes);
}

void BombDialogView::SetUp() {
    switch (mUserServices.GetSettingsService().GetControlType()) {
        case ControlType::Click:
            mHandPhtAnimation->SetActiveClip(0);
            mBombAnimation->SetActiveClip(0);
            mBlastRadiusPhtAnimation->SetActiveClip(0);
            mBombMoves->SetIsVisible(true);
            mBombGhostPieceContainer->SetIsVisible(false);
            break;
        case ControlType::Gesture:
            mHandPhtAnimation->SetActiveClip(1);
            mBombAnimation->SetActiveClip(1);
            mBlastRadiusPhtAnimation->SetActiveClip(1);
            mBombMoves->SetIsVisible(false);
            mBombGhostPieceContainer->SetIsVisible(true);
            break;
    }

    if (mGuiLightProvider) {
        TutorialUtils::SetGuiLightDirections(*mGuiLightProvider);
    }
    
    mBlastRadius->GetRenderable()->GetMaterial().SetOpacity(0.95f);
    mExplosionEffect->SetUp();
    mAnimation->Stop();
    mAnimation->Play();
}

void BombDialogView::OnDeactivate() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

void BombDialogView::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    mAnimation->Update(dt);
    mExplosionEffect->Update(dt);
    mHandAnimation->Update();
}
