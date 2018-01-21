#include "GameHud.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"

// Game includes.
#include "GameLogic.hpp"
#include "LevelResources.hpp"
#include "GameHudController.hpp"

using namespace BlocksGame;

namespace {
    const Pht::Vec3 lightDirectionA {0.6f, 1.0f, 1.0f};
    const Pht::Vec3 lightDirectionB {0.4f, 1.0f, 1.0f};
    const auto lightAnimationDuration {5.0f};
}

GameHud::GameHud(Pht::IEngine& engine,
                 const GameLogic& gameLogic,
                 const LevelResources& levelResources,
                 GameHudController& gameHudController) :
    mEngine {engine},
    mGameLogic {gameLogic},
    mLPiece {levelResources.GetLPiece()},
    mGrayBlock {levelResources.GetLevelBlockRenderable(BlockRenderableKind::Full)},
    mBlueprintSlot {levelResources.GetBlueprintSlotRenderable()},
    mProgressPosition {-4.1f, 12.6f},
    mMovesPosition {3.1f, 12.6f},
    mNextPiecesPosition {-2.3f, -12.3f},
    mSelectablePiecesPosition {3.1f, -12.3f},
    mFont {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(22)},
    mProgressText {
        {mProgressPosition.x + 1.1f, mProgressPosition.y},
        "    ",  // Warning! Must be four spaces to fit digits.
        {mFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}}
    },
    mMovesText {
        {mMovesPosition.x + 1.1f, mMovesPosition.y},
        "   ",   // Warning! Must be three spaces to fit digits.
        {mFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}}
    },
    mNextText {
        {mNextPiecesPosition.x - 0.1f, mNextPiecesPosition.y + 1.22f},
        "NEXT",
        {mFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}}
    },
    mSwitchText {
        {mSelectablePiecesPosition.x - 0.55f, mSelectablePiecesPosition.y + 1.22f},
        "SWITCH",
        {mFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}}
    },
    mLPieceRelativePosition {0.12f, 0.2f},
    mGrayBlockRelativePosition {0.19f, 0.2f},
    mBlueprintSlotRelativePosition {0.55f, 0.2f} {
    
    mPieceRelativePositions = {
        Pht::Vec2{-0.85f, 0.12f},
        Pht::Vec2{1.15f, 0.12f}
    };
    
    engine.GetRenderer().SetHudCameraPosition({0.0f, 0.0f, 15.5f});

    mProgressTextRectangle = CreateTextRectangle(Pht::Vec3 {mProgressPosition.x + 0.9f,
                                                            mProgressPosition.y + 0.2f,
                                                            -3.0f},
                                                 4.4f,
                                                 false);
    mMovesTextRectangle = CreateTextRectangle(Pht::Vec3 {mMovesPosition.x + 0.9f,
                                                         mMovesPosition.y + 0.2f,
                                                         -3.0f},
                                              4.4f,
                                              false);
    mProgressPiecesRectangle = CreateSmallPiecesRectangle(Pht::Vec3 {mProgressPosition.x + 0.15f,
                                                                     mProgressPosition.y + 0.1f,
                                                                     -2.0f});
    mMovesPiecesRectangle = CreateSmallPiecesRectangle(Pht::Vec3 {mMovesPosition.x + 0.15f,
                                                                  mMovesPosition.y + 0.1f,
                                                                  -2.0f});
    mNextPiecesRectangle = CreatePiecesRectangle(Pht::Vec3 {mNextPiecesPosition.x + 0.15f,
                                                            mNextPiecesPosition.y,
                                                            -2.0f},
                                                 false);
        
    Pht::Vec3 selectablePiecesRectanglePosition {
        mSelectablePiecesPosition.x + 0.15f,
        mSelectablePiecesPosition.y,
        -2.0f
    };
    
    mSelectablePiecesRectangle = CreatePiecesRectangle(selectablePiecesRectanglePosition, false);
    mSelectablePiecesRectangleBright = CreatePiecesRectangle(selectablePiecesRectanglePosition, true);
    
    mNextTextRectangle = CreateTextRectangle(Pht::Vec3 {mNextPiecesPosition.x + 0.55f,
                                                        mNextPiecesPosition.y + 1.42f,
                                                        0.0f},
                                             4.8f,
                                             false);
    
    Pht::Vec3 switchTextRectanglePosition {
        mSelectablePiecesPosition.x + 0.55f,
        mSelectablePiecesPosition.y + 1.42f,
        0.0f
    };
        
    mSwitchTextRectangle = CreateTextRectangle(switchTextRectanglePosition, 4.8f, false);
    mSwitchTextRectangleBright = CreateTextRectangle(switchTextRectanglePosition, 4.8f, true);
        
    gameHudController.SetHudEventListener(*this);
}

std::unique_ptr<GradientRectangle> GameHud::CreatePiecesRectangle(const Pht::Vec3& position,
                                                                  bool isBright) {
    Pht::Vec2 size {4.8f, 2.2f};
    auto tilt {0.50f};
    float leftQuadWidth {0.4f};
    float rightQuadWidth {0.4f};
    
    GradientRectangleColors upperColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {isBright ? 0.95f : 0.85f, isBright ? 0.6f : 0.4f, 0.95f, 0.93f}, // .mMid = {0.85f, 0.7f, 0.95f, 0.93f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };

    GradientRectangleColors lowerColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {0.9f, 0.9f, 1.0f, 0.0f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };
    
    return std::make_unique<GradientRectangle>(mEngine,
                                               mSceneResources,
                                               position,
                                               size,
                                               tilt,
                                               leftQuadWidth,
                                               rightQuadWidth,
                                               upperColors,
                                               lowerColors);
}

std::unique_ptr<GradientRectangle> GameHud::CreateSmallPiecesRectangle(const Pht::Vec3& position) {
    Pht::Vec2 size {1.8f, 1.3f};
    auto tilt {0.23f};
    float leftQuadWidth {0.3f};
    float rightQuadWidth {0.3f};
    
    GradientRectangleColors upperColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {0.85f, 0.75f, 0.95f, 0.9f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };

    GradientRectangleColors lowerColors {
        .mLeft = {0.9f, 0.9f, 1.0f, 0.0f},
        .mMid = {0.9f, 0.9f, 1.0f, 0.0f},
        .mRight = {0.9f, 0.9f, 1.0f, 0.0f}
    };
    
    return std::make_unique<GradientRectangle>(mEngine,
                                               mSceneResources,
                                               position,
                                               size,
                                               tilt,
                                               leftQuadWidth,
                                               rightQuadWidth,
                                               upperColors,
                                               lowerColors);
}

std::unique_ptr<GradientRectangle> GameHud::CreateTextRectangle(const Pht::Vec3& position,
                                                                float length,
                                                                bool isBright) {
    Pht::Vec2 size {length, 0.7f};
    float leftQuadWidth {1.0f};
    float rightQuadWidth {1.0f};
    
    GradientRectangleColors colors {
        .mLeft = {0.6f, 0.3f, 0.75f, 0.0f},
        .mMid = {isBright ? 0.7f : 0.6f, isBright ? 0.5f : 0.3f, isBright ? 0.85f : 0.75f, 0.8f},
        .mRight = {0.6f, 0.3f, 0.75f, 0.0f}
    };
    
    return std::make_unique<GradientRectangle>(mEngine,
                                               mSceneResources,
                                               position,
                                               size,
                                               0.0f,
                                               leftQuadWidth,
                                               rightQuadWidth,
                                               colors,
                                               colors);
}

void GameHud::OnSwitchButtonDown() {
    mIsSwitchButtonDown = true;
}

void GameHud::OnSwitchButtonUp() {
    mIsSwitchButtonDown = false;
}

void GameHud::Reset(const Level& level) {
    mLevelObjective = level.GetObjective();
}

void GameHud::Update() {
    UpdateLightAnimation();
    UpdateProgress();
    UpdateMovesLeft();
}

void GameHud::UpdateLightAnimation() {
    mLightAnimationTime += mEngine.GetLastFrameSeconds();
    
    if (mLightAnimationTime > lightAnimationDuration) {
        mLightAnimationTime = 0.0f;
    }
    
    auto t {(sin(mLightAnimationTime * 2.0f * 3.1415f / lightAnimationDuration) + 1.0f) / 2.0f};
    mLightDirection = lightDirectionA.Lerp(t, lightDirectionB);
}

void GameHud::UpdateProgress() {
    auto progress {
        mLevelObjective == Level::Objective::Clear ? mGameLogic.GetNumLevelBlocksLeft() :
                                                     mGameLogic.GetNumEmptyBlueprintSlotsLeft()
    };
    
    if (progress != mProgress) {
        const auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%4d", progress);
        auto numDigits {std::strlen(buffer)};
        assert(numDigits <= 4);
        
        auto& text {mProgressText.mText};
        auto textLength {text.size()};
        assert(textLength == 4);
        text[0] = buffer[0];
        text[1] = buffer[1];
        text[2] = buffer[2];
        text[3] = buffer[3];
        
        mProgress = progress;
    }
}

void GameHud::UpdateMovesLeft() {
    auto movesLeft {mGameLogic.GetMovesLeft()};
    
    if (movesLeft != mMovesLeft) {
        const auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%3d", movesLeft);
        auto numDigits {std::strlen(buffer)};
        assert(numDigits <= 3);
        
        auto& text {mMovesText.mText};
        auto textLength {text.size()};
        assert(textLength == 3);
        text[0] = buffer[0];
        text[1] = buffer[1];
        text[2] = buffer[2];
        
        mMovesLeft = movesLeft;
    }
}

const Pht::RenderableObject* GameHud::GetGrayBlock() const {
    switch (mLevelObjective) {
        case Level::Objective::Clear:
            return &mGrayBlock;
        case Level::Objective::Build:
            return nullptr;
    }
}

const Pht::RenderableObject* GameHud::GetBlueprintSlot() const {
    switch (mLevelObjective) {
        case Level::Objective::Clear:
            return nullptr;
        case Level::Objective::Build:
            return &mBlueprintSlot;
    }
}

const GradientRectangle& GameHud::GetSelectablePiecesRectangle() const {
    if (mIsSwitchButtonDown) {
        return *mSelectablePiecesRectangleBright;
    } else {
        return *mSelectablePiecesRectangle;
    }
}

const GradientRectangle& GameHud::GetSwitchTextRectangle() const {
    if (mIsSwitchButtonDown) {
        return *mSwitchTextRectangleBright;
    } else {
        return *mSwitchTextRectangle;
    }
}
