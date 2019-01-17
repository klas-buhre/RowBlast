#include "MapHudController.hpp"

// Game includes:
#include "CommonResources.hpp"

using namespace RowBlast;

MapHudController::MapHudController(Pht::IEngine& engine, const CommonResources& commonResources) :
    mView {engine, commonResources} {}

MapHudController::Result MapHudController::OnTouch(const Pht::TouchEvent& event) {
    auto& optionsButton {mView.GetOptionsButton()};

    if (optionsButton.IsClicked(event)) {
        return Result::ClickedOptionsButton;
    }
    
    if (optionsButton.GetButton().StateIsDownOrMovedOutside()) {
        return Result::TouchStartedOverButton;
    }

    auto& coinsButton {mView.GetCoinsButton()};

    if (coinsButton.IsClicked(event)) {
        return Result::ClickedCoinsButton;
    }
    
    if (coinsButton.GetButton().StateIsDownOrMovedOutside()) {
        return Result::TouchStartedOverButton;
    }

    auto& livesButton {mView.GetLivesButton()};

    if (livesButton.IsClicked(event)) {
        return Result::ClickedLivesButton;
    }
    
    if (livesButton.GetButton().StateIsDownOrMovedOutside()) {
        return Result::TouchStartedOverButton;
    }

    return Result::None;
}
