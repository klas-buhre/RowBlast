#include "GameHudController.hpp"

// Engine includes.
#include "Button.hpp"
#include "IEngine.hpp"
#include "IAudio.hpp"
#include "IRenderer.hpp"

// Game includes:
#include "CommonResources.hpp"
#include "IGameHudEventListener.hpp"

using namespace BlocksGame;

GameHudController::GameHudController(Pht::IEngine& engine) :
    mView {engine},
    mEngine {engine} {}

GameHudController::Result GameHudController::OnTouch(const Pht::TouchEvent& event) {
    auto& pauseButton {mView.GetPauseButton()};
    auto& switchButton {mView.GetSwitchButton()};
    
    if (IsSwitchButtonClicked(event)) {
        return Result::ClickedSwitch;
    }

    if (pauseButton.IsClicked(event)) {
        mEngine.GetAudio().PlaySound(CommonResources::mBlipSound);
        return Result::ClickedPause;
    }
    
    if (pauseButton.StateIsDownOrMovedOutside() || switchButton.StateIsDownOrMovedOutside()) {
        return Result::TouchStartedOverButton;
    }
    
    return Result::None;
}

bool GameHudController::IsSwitchButtonClicked(const Pht::TouchEvent& event) const {
    auto result {false};
    auto& renderer {mEngine.GetRenderer()};
    renderer.SetHudMode(true);

    switch (mView.GetSwitchButton().OnTouch(event)) {
        case Pht::Button::Result::Down:
            if (mGameHudEventListener) {
                mGameHudEventListener->OnSwitchButtonDown();
            }
            break;
        case Pht::Button::Result::UpInside:
            if (mGameHudEventListener) {
                mGameHudEventListener->OnSwitchButtonUp();
            }
            result = true;
            break;
        case Pht::Button::Result::UpOutside:
        case Pht::Button::Result::MoveOutside:
            if (mGameHudEventListener) {
                mGameHudEventListener->OnSwitchButtonUp();
            }
            break;
        case Pht::Button::Result::MoveInside:
        case Pht::Button::Result::None:
            break;
    }
    
    renderer.SetHudMode(false);
    return result;
}
