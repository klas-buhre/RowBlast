#include "LevelBombDialogView.hpp"

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
    constexpr auto animationDuration = 4.25f;
    constexpr auto clickMoveTime = 1.5f;
    constexpr auto piecePosition2Time = 1.65f;
    constexpr auto detonationTime = 1.95f;
    constexpr auto beginSwipeRightTime = 1.0f;
    constexpr auto beginSwipeDownTime = 1.5f;
    constexpr auto blockFlyDuration = 0.5f;
    constexpr auto blockFallWaitDuration = 0.75f;
    constexpr auto blockFallDuration = 0.6f;
    constexpr auto explosionSpreadDelay = 0.15f;
}

LevelBombDialogView::LevelBombDialogView(Pht::IEngine& engine,
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
    CreateText({-1.6f, 8.45f, UiLayer::text}, "BOMBS", largeTextProperties);

    GuiUtils::CreateTitleBarLine(engine, *this, 2.2f);
    
    CreateAnimation(pieceResources, levelResources);

    auto& textProperties = guiResources.GetSmallWhiteTextProperties(zoom);
    CreateText({-4.85f, -4.8f, UiLayer::panel}, "Detonate bombs by placing a", textProperties);
    CreateText({-2.3f, -5.875f, UiLayer::panel}, "piece on them", textProperties);
    
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

void LevelBombDialogView::CreateAnimation(const PieceResources& pieceResources,
                                          const LevelResources& levelResources) {
    auto& container = CreateSceneObject();
    container.GetTransform().SetPosition({0.0f, 1.8f, 0.0f});
    container.GetTransform().SetScale(1.25f);
    GetRoot().AddChild(container);
    
    auto& animationSystem = mEngine.GetAnimationSystem();
    mAnimation = &animationSystem.CreateAnimation(container, {{.mTime = 0.0f}, {.mTime = animationDuration}});

    TutorialUtils::CreateFieldQuad(mEngine, *this, container);
    TutorialUtils::CreateFieldQuadFrame(mEngine, *this, container);
    
    auto& grayBlock0 = TutorialUtils::CreateGrayBlock(*this, {-3.0f, 0.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock1 = TutorialUtils::CreateGrayBlock(*this, {0.0f, 0.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock2 = TutorialUtils::CreateGrayBlock(*this, {1.0f, 0.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock3 = TutorialUtils::CreateGrayBlock(*this, {2.0f, 0.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock4 = TutorialUtils::CreateGrayBlock(*this, {-3.0f, -1.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock5 = TutorialUtils::CreateGrayBlock(*this, {3.0f, -1.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock6 = TutorialUtils::CreateGrayBlock(*this, {-3.0f, -2.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock7 = TutorialUtils::CreateGrayBlock(*this, {3.0f, -2.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock8 = TutorialUtils::CreateGrayBlock(*this, {-2.0f, -3.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock9 = TutorialUtils::CreateGrayBlock(*this, {-1.0f, -3.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock10 = TutorialUtils::CreateGrayBlock(*this, {0.0f, -3.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock11 = TutorialUtils::CreateGrayBlock(*this, {1.0f, -3.0f, UiLayer::block}, container, levelResources);
    auto& grayBlock12 = TutorialUtils::CreateGrayBlock(*this, {2.0f, -3.0f, UiLayer::block}, container, levelResources);

    mLevelBombs = &CreateSceneObject();
    container.AddChild(*mLevelBombs);
    
    auto& bomb0 = TutorialUtils::CreateLevelBomb(*this, {-2.0, -1.0f, UiLayer::block}, *mLevelBombs, levelResources);
    auto& bomb1 = TutorialUtils::CreateLevelBomb(*this, {-1.0, -1.0f, UiLayer::block}, *mLevelBombs, levelResources);
    auto& bomb2 = TutorialUtils::CreateLevelBomb(*this, {0.0, -1.0f, UiLayer::block}, *mLevelBombs, levelResources);
    auto& bomb3 = TutorialUtils::CreateLevelBomb(*this, {1.0, -1.0f, UiLayer::block}, *mLevelBombs, levelResources);
    auto& bomb4 = TutorialUtils::CreateLevelBomb(*this, {2.0, -1.0f, UiLayer::block}, *mLevelBombs, levelResources);
    auto& bomb5 = TutorialUtils::CreateLevelBomb(*this, {-2.0, -2.0f, UiLayer::block}, *mLevelBombs, levelResources);
    auto& bomb6 = TutorialUtils::CreateLevelBomb(*this, {-1.0, -2.0f, UiLayer::block}, *mLevelBombs, levelResources);
    auto& bomb7 = TutorialUtils::CreateLevelBomb(*this, {0.0, -2.0f, UiLayer::block}, *mLevelBombs, levelResources);
    auto& bomb8 = TutorialUtils::CreateLevelBomb(*this, {1.0, -2.0f, UiLayer::block}, *mLevelBombs, levelResources);
    auto& bomb9 = TutorialUtils::CreateLevelBomb(*this, {2.0, -2.0f, UiLayer::block}, *mLevelBombs, levelResources);
    
    std::vector<Pht::Keyframe> bombWave1Keyframes {
        {
            .mTime = 0.0f,
            .mIsVisible = true
        },
        {
            .mTime = detonationTime,
            .mIsVisible = false,
            .mCallback = [this] () {
                mExplosionEffects[0].StartExplosion({-2.0, -1.0f, TutorialUiLayer::bomb});
                mExplosionEffects[1].StartExplosion({-1.0, -1.0f, TutorialUiLayer::bomb});
            }
        },
        {
            .mTime = animationDuration
        }
    };
    animationSystem.CreateAnimation(bomb0, bombWave1Keyframes);
    animationSystem.CreateAnimation(bomb1, bombWave1Keyframes);

    std::vector<Pht::Keyframe> bombWave2Keyframes {
        {
            .mTime = 0.0f,
            .mIsVisible = true
        },
        {
            .mTime = detonationTime + explosionSpreadDelay,
            .mIsVisible = false,
            .mCallback = [this] () {
                mExplosionEffects[5].StartExplosion({-2.0, -2.0f, TutorialUiLayer::bomb});
                mExplosionEffects[6].StartExplosion({-1.0, -2.0f, TutorialUiLayer::bomb});
                mExplosionEffects[2].StartExplosion({0.0, -1.0f, TutorialUiLayer::bomb});
                mExplosionEffects[7].StartExplosion({0.0, -2.0f, TutorialUiLayer::bomb});
            }
        },
        {
            .mTime = animationDuration
        }
    };
    animationSystem.CreateAnimation(bomb5, bombWave2Keyframes);
    animationSystem.CreateAnimation(bomb6, bombWave2Keyframes);
    animationSystem.CreateAnimation(bomb2, bombWave2Keyframes);
    animationSystem.CreateAnimation(bomb7, bombWave2Keyframes);

    std::vector<Pht::Keyframe> bombWave3Keyframes {
        {
            .mTime = 0.0f,
            .mIsVisible = true
        },
        {
            .mTime = detonationTime + 2.0f * explosionSpreadDelay,
            .mIsVisible = false,
            .mCallback = [this] () {
                mExplosionEffects[8].StartExplosion({1.0, -2.0f, TutorialUiLayer::bomb});
                mExplosionEffects[3].StartExplosion({1.0, -1.0f, TutorialUiLayer::bomb});
            }
        },
        {
            .mTime = animationDuration
        }
    };
    animationSystem.CreateAnimation(bomb8, bombWave3Keyframes);
    animationSystem.CreateAnimation(bomb3, bombWave3Keyframes);

    std::vector<Pht::Keyframe> bombWave4Keyframes {
        {
            .mTime = 0.0f,
            .mIsVisible = true
        },
        {
            .mTime = detonationTime + 3.0f * explosionSpreadDelay,
            .mIsVisible = false,
            .mCallback = [this] () {
                mExplosionEffects[9].StartExplosion({2.0, -2.0f, TutorialUiLayer::bomb});
                mExplosionEffects[4].StartExplosion({2.0, -1.0f, TutorialUiLayer::bomb});
            }
        },
        {
            .mTime = animationDuration
        }
    };
    animationSystem.CreateAnimation(bomb9, bombWave4Keyframes);
    animationSystem.CreateAnimation(bomb4, bombWave4Keyframes);

    TutorialUtils::CreateThreeGrayBlocks(*this, {2.0f, -4.0f, UiLayer::block}, container, levelResources);
    
    mMoves = &CreateSceneObject();
    container.AddChild(*mMoves);
    auto& move1 = TutorialUtils::CreatePieceGhostPiece(*this, "L", {-1.5f, 0.5f, UiLayer::block}, 0.0f, *mMoves, levelResources);
    auto& move2 = TutorialUtils::CreatePieceGhostPiece(*this, "L", {0.5f, 1.5f, UiLayer::block}, 0.0f, *mMoves, levelResources);
    auto& move3 = TutorialUtils::CreatePieceGhostPiece(*this, "L", {2.5f, 0.5f, UiLayer::block}, 90.0f, *mMoves, levelResources);
    
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
    auto& ghostPiece = TutorialUtils::CreatePieceGhostPiece(*this, "L", ghostPieceInitialPosition, 0.0f, *mGhostPieceContainer, levelResources);

    std::vector<Pht::Keyframe> ghostPieceKeyframes {
        {.mTime = 0.0f, .mPosition = ghostPieceInitialPosition, .mIsVisible = true},
        {.mTime = beginSwipeRightTime, .mPosition = ghostPieceInitialPosition},
        {.mTime = beginSwipeDownTime, .mPosition = ghostPiecePosition2},
        {.mTime = detonationTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    auto& ghostPieceAnimation = animationSystem.CreateAnimation(ghostPiece, ghostPieceKeyframes);
    ghostPieceAnimation.SetInterpolation(Pht::Interpolation::None);

    Pht::Vec3 pieceInitialPosition {-0.5f, 3.3f, UiLayer::block};
    Pht::Vec3 piecePosition2 {-1.5f, 3.3f, UiLayer::block};
    Pht::Vec3 pieceLandingPosition {-1.5f, 0.5f, UiLayer::block};
    
    auto& piece = TutorialUtils::CreateLPiece(*this, pieceInitialPosition, 0.0f, container, pieceResources);
    
    std::vector<Pht::Keyframe> pieceKeyframes {
        {.mTime = 0.0f, .mPosition = pieceInitialPosition, .mIsVisible = true},
        {.mTime = clickMoveTime, .mPosition = pieceInitialPosition},
        {.mTime = piecePosition2Time, .mPosition = piecePosition2},
        {.mTime = detonationTime, .mPosition = pieceLandingPosition, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    mPieceAnimation = &animationSystem.CreateAnimation(piece, pieceKeyframes);
    
    std::vector<Pht::Keyframe> pieceSwipeKeyframes {
        {.mTime = 0.0f, .mPosition = pieceInitialPosition, .mIsVisible = true},
        {.mTime = beginSwipeRightTime, .mPosition = pieceInitialPosition},
        {.mTime = beginSwipeDownTime, .mPosition = piecePosition2},
        {.mTime = detonationTime, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    Pht::AnimationClip pieceSwipeClip {pieceSwipeKeyframes};
    pieceSwipeClip.SetInterpolation(Pht::Interpolation::None);
    mPieceAnimation->AddClip(pieceSwipeClip, 1);
    
    auto& yellowBlocks = CreateSceneObject();
    container.AddChild(yellowBlocks);
    
    std::vector<Pht::Keyframe> yellowBlocksKeyframes {
        {.mTime = 0.0f, .mIsVisible = false},
        {.mTime = detonationTime, .mIsVisible = true},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(yellowBlocks, yellowBlocksKeyframes);
    
    auto& yellowBlock1 = TutorialUtils::CreateColoredBlock(*this, {-1.0f, 1.0f, UiLayer::block}, BlockColor::Yellow, yellowBlocks, pieceResources);
    auto& yellowBlock2 = TutorialUtils::CreateColoredBlock(*this, {-2.0f, 0.0f, UiLayer::block}, BlockColor::Yellow, yellowBlocks, pieceResources);
    auto& yellowBlock3 = TutorialUtils::CreateColoredBlock(*this, {-1.0f, 0.0f, UiLayer::block}, BlockColor::Yellow, yellowBlocks, pieceResources);

    std::vector<Pht::Keyframe> yellowBlock1Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-1.0f, 1.0f, UiLayer::block}, .mIsVisible = true},
        {.mTime = detonationTime + blockFallWaitDuration, .mPosition = Pht::Vec3{-1.0f, 1.0f, UiLayer::block}},
        {.mTime = detonationTime + blockFallWaitDuration + blockFallDuration, .mPosition = Pht::Vec3{-1.0f, -4.0f, UiLayer::block}},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(yellowBlock1, yellowBlock1Keyframes);

    std::vector<Pht::Keyframe> yellowBlock2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-2.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{-2.0f, 0.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-2.5f, 2.5f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(yellowBlock2, yellowBlock2Keyframes);

    std::vector<Pht::Keyframe> yellowBlock3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-1.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{-1.0f, 0.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-1.0f, 3.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(yellowBlock3, yellowBlock3Keyframes);
    
    mHandAnimation = std::make_unique<HandAnimation>(mEngine, 1.0f, true);
    mHandAnimation->Init(container);
    
    std::vector<Pht::Keyframe> handAnimationClickKeyframes {
        {
            .mTime = 0.0f,
            .mCallback = [this] () {
                mHandAnimation->StartInNotTouchingScreenState({-2.4f, 0.6f, UiLayer::root},
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
                mHandAnimation->BeginTouch(detonationTime - beginSwipeRightTime);
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
    
    for (auto i = 0; i < 10; ++i) {
        mExplosionEffects.emplace_back(mEngine, TutorialExplosionParticleEffect::Kind::LevelBomb, container);
    }
    
    std::vector<Pht::Keyframe> grayBlock0Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-3.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{-3.0f, 0.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-4.5f, 1.5f, -0.05f}, .mRotation = Pht::Vec3{50.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock0, grayBlock0Keyframes);

    std::vector<Pht::Keyframe> grayBlock1Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + explosionSpreadDelay, .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + explosionSpreadDelay, .mPosition = Pht::Vec3{0.0f, 3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock1, grayBlock1Keyframes);
    
    std::vector<Pht::Keyframe> grayBlock2Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{1.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 2.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{1.0f, 0.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 2.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{1.0f, 3.0f, 0.0f}, .mRotation = Pht::Vec3{50.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock2, grayBlock2Keyframes);

    std::vector<Pht::Keyframe> grayBlock3Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, 0.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 3.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{2.0f, 0.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 3.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{2.0f, 3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock3, grayBlock3Keyframes);

    std::vector<Pht::Keyframe> grayBlock4Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-3.0f, -1.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f, .mPosition = Pht::Vec3{-3.0f, -1.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration, .mPosition = Pht::Vec3{-5.5f, -1.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock4, grayBlock4Keyframes);

    std::vector<Pht::Keyframe> grayBlock5Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{3.0f, -1.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 3.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{3.0f, -1.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 3.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{5.5f, -1.0f, TutorialUiLayer::root}, .mRotation = Pht::Vec3{50.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock5, grayBlock5Keyframes);

    std::vector<Pht::Keyframe> grayBlock6Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-3.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 1.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{-3.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 1.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{-5.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock6, grayBlock6Keyframes);

    std::vector<Pht::Keyframe> grayBlock7Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{3.0f, -2.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 3.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{3.0f, -2.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 3.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{6.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 50.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock7, grayBlock7Keyframes);

    std::vector<Pht::Keyframe> grayBlock8Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-2.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 1.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{-2.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 1.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{-3.0f, -5.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock8, grayBlock8Keyframes);

    std::vector<Pht::Keyframe> grayBlock9Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{-1.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 1.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{-1.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 1.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{-1.0f, -6.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock9, grayBlock9Keyframes);

    std::vector<Pht::Keyframe> grayBlock10Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{0.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 1.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{0.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 1.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{0.0f, -6.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 100.0f, 0.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock10, grayBlock10Keyframes);

    std::vector<Pht::Keyframe> grayBlock11Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{1.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 2.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{1.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 2.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{1.0f, -6.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 100.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock11, grayBlock11Keyframes);

    std::vector<Pht::Keyframe> grayBlock12Keyframes {
        {.mTime = 0.0f, .mPosition = Pht::Vec3{2.0f, -3.0f, UiLayer::block}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}, .mIsVisible = true},
        {.mTime = detonationTime + 0.1f + 3.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{2.0f, -3.0f, 0.0f}, .mRotation = Pht::Vec3{0.0f, 0.0f, 0.0f}},
        {.mTime = detonationTime + blockFlyDuration + 3.0f * explosionSpreadDelay, .mPosition = Pht::Vec3{3.0f, -5.0f, 0.0f}, .mRotation = Pht::Vec3{100.0f, 0.0f, 100.0f}, .mIsVisible = false},
        {.mTime = animationDuration}
    };
    animationSystem.CreateAnimation(grayBlock12, grayBlock12Keyframes);
}

void LevelBombDialogView::SetUp() {
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
    
    for (auto& explosionEffect: mExplosionEffects) {
        explosionEffect.SetUp();
    }
    
    mAnimation->Stop();
    mAnimation->Play();
}

void LevelBombDialogView::OnDeactivate() {
    if (mGuiLightProvider) {
        mGuiLightProvider->SetDefaultGuiLightDirections();
    }
}

void LevelBombDialogView::Update() {
    auto dt = mEngine.GetLastFrameSeconds();
    mAnimation->Update(dt);
    mHandAnimation->Update();
    
    for (auto& explosionEffect: mExplosionEffects) {
        explosionEffect.Update(dt);
    }
}
