#include "Tutorial.hpp"

#include <assert.h>

// Game includes.
#include "GameScene.hpp"
#include "Level.hpp"

using namespace RowBlast;

Tutorial::Tutorial(Pht::IEngine& engine, GameScene& scene, const CommonResources& commonResources) :
    mScene {scene},
    mPlacePieceWindowController {engine, commonResources} {
    
    mViewManager.AddView(static_cast<int>(Controller::PlacePieceWindow), mPlacePieceWindowController.GetView());
}

void Tutorial::Init(const Level& level) {
    mLevel = &level;
    
    if (!level.IsPartOfTutorial()) {
        return;
    }

    auto& uiViewContainer {mScene.GetUiViewsContainer()};
    mViewManager.Init(uiViewContainer);
    SetActiveController(Controller::None);
}

void Tutorial::SetActiveController(Controller controller) {
    mActiveController = controller;

    if (controller == Controller::None) {
        mViewManager.DeactivateAllViews();
    } else {
        mViewManager.ActivateView(static_cast<int>(controller));
    }
}

void Tutorial::Update() {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }

    switch (mActiveController) {
        case Controller::PlacePieceWindow:
            mPlacePieceWindowController.Update();
            break;
        case Controller::None:
            break;
    }
}

void Tutorial::OnNextMove(int numMovesUsedIncludingCurrent) {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }
    
    switch (mLevel->GetId()) {
        case 1:
            OnNextMoveFirstLevel(numMovesUsedIncludingCurrent);
            break;
        default:
            assert(!"Unsupported tutorial level");
    }

}

void Tutorial::OnNextMoveFirstLevel(int numMovesUsedIncludingCurrent) {
    switch (numMovesUsedIncludingCurrent) {
        case 1:
            SetActiveController(Controller::PlacePieceWindow);
            mPlacePieceWindowController.Init();
            break;
        default:
            assert(!"Unsupported number of used moves");
    }
}

void Tutorial::OnSwitchPiece(const Piece& pieceType) {
    if (!mLevel->IsPartOfTutorial()) {
        return;
    }

}

bool Tutorial::IsSwitchPieceAllowed() const {
    if (!mLevel->IsPartOfTutorial()) {
        return true;
    }
    
    return false;
}

bool Tutorial::IsPlacePieceAllowed(const Piece& pieceType) const {
    if (!mLevel->IsPartOfTutorial()) {
        return true;
    }

    return false;
}
