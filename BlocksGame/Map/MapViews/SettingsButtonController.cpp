#include "SettingsButtonController.hpp"

// Engine includes.
#include "Button.hpp"
#include "IEngine.hpp"
#include "IAudio.hpp"

// Game includes:
#include "CommonResources.hpp"

using namespace RowBlast;

SettingsButtonController::SettingsButtonController(Pht::IEngine& engine) :
    mView {engine},
    mAudio {engine.GetAudio()} {}

SettingsButtonController::Result SettingsButtonController::OnTouch(const Pht::TouchEvent& event) {
    auto& button {mView.GetButton()};

    if (button.IsClicked(event)) {
        mAudio.PlaySound(CommonResources::mBlipSound);
        return Result::ClickedSettings;
    }
    
    if (button.StateIsDownOrMovedOutside()) {
        return Result::TouchStartedOverButton;
    }
    
    return Result::None;
}
