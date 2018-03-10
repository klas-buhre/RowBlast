#include "PreviewPiecesAnimation.hpp"

// Game includes.
#include "GameScene.hpp"
#include "GameLogic.hpp"

using namespace BlocksGame;

PreviewPiecesAnimation::PreviewPiecesAnimation(GameScene& scene, GameLogic& gameLogic) :
    mScene {scene},
    mGameLogic {gameLogic} {}

void PreviewPiecesAnimation::Update(float dt) {
    auto& hud {mScene.GetHud()};
    
    switch (mGameLogic.GetPreviewPieceAnimationToStart()) {
        case PreviewPieceAnimationToStart::NextPieceAndSwitch:
            std::cout << "Should start next piece and switch animation" << std::endl;
            hud.OnNextPieceAnimationFinished();
            hud.OnSwitchPieceAnimationFinished();
            break;
        case PreviewPieceAnimationToStart::SwitchPiece:
            std::cout << "Should start switch piece animation" << std::endl;
            hud.OnSwitchPieceAnimationFinished();
            break;
        case PreviewPieceAnimationToStart::None:
            break;
    }
    
    mGameLogic.ResetPreviewPieceAnimationToStart();
}
