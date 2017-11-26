#include "MapController.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "InputEvent.hpp"
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "UserData.hpp"

using namespace BlocksGame;

namespace {
    auto cameraDeaccelerationFactor {0.9f};
    Pht::Mat4 identityMatrix;
}

MapController::Command::Command(Kind kind, int level) :
    mKind {kind},
    mLevel {level} {}

int MapController::Command::GetLevel() const {
    assert(mKind == StartGame);
    return mLevel;
}

MapController::MapController(Pht::IEngine& engine,
                             const CommonResources& commonResources,
                             CommonViewControllers& commonViewControllers,
                             UserData& userData) :
    mEngine {engine},
    mUserData {userData},
    mScene {engine, commonResources, userData},
    mHud {engine, userData},
    mMapViewControllers {engine, commonViewControllers},
    mRenderer {engine.GetRenderer(), mScene, mHud, mMapViewControllers, userData} {}

void MapController::Reset() {
    mScene.Reset();
}

void MapController::Stop() {
    mScene.Stop();
}

MapController::Command MapController::Update() {
    auto command {Command{Command::None}};
    
    switch (mState) {
        case State::Map:
            command = UpdateMap();
            break;
        case State::NoLivesDialog:
            UpdateNoLivesDialog();
            break;
        case State::SettingsMenu:
            UpdateSettingsMenu();
            break;
    }

    mHud.Update();
    mScene.Update();
    
    return command;
}

void MapController::RenderScene() {
    mRenderer.RenderFrame();
}

MapController::Command MapController::UpdateMap() {
    auto command {HandleInput()};
    
    if (!mIsTouching) {
        UpdateCamera();
    }
    
    return command;
}

void MapController::UpdateNoLivesDialog() {
    switch (mMapViewControllers.GetNoLivesDialogController().Update()) {
        case NoLivesDialogController::Result::None:
            break;
        case NoLivesDialogController::Result::RefillLives:
            mUserData.GetLifeManager().RefillLives();
            mState = State::Map;
            mMapViewControllers.SetActiveController(MapViewControllers::SettingsButton);
            break;
        case NoLivesDialogController::Result::Close:
            mState = State::Map;
            mMapViewControllers.SetActiveController(MapViewControllers::SettingsButton);
            break;
    }
}

void MapController::UpdateSettingsMenu() {
    switch (mMapViewControllers.GetSettingsMenuController().Update()) {
        case SettingsMenuController::Result::None:
            break;
        case SettingsMenuController::Result::GoBack:
            mState = State::Map;
            mMapViewControllers.SetActiveController(MapViewControllers::SettingsButton);
            break;
    }
}

MapController::Command MapController::HandleInput() {
    auto command {Command{Command::None}};
    auto& input {mEngine.GetInput()};
    
    while (input.HasEvents()) {
        auto& event {input.GetNextEvent()};
        switch (event.GetKind()) {
            case Pht::InputKind::Touch: {
                auto& touchEvent {event.GetTouchEvent()};
                switch (mMapViewControllers.GetSettingsButtonController().OnTouch(touchEvent)) {
                    case SettingsButtonController::Result::None:
                        command = HandleTouch(touchEvent);
                        break;
                    case SettingsButtonController::Result::ClickedSettings:
                        GoToSettingsMenuState();
                        break;
                    case SettingsButtonController::Result::TouchStartedOverButton:
                        break;
                }            
                break;
            }
            default:
                assert(false);
                break;
        }
        
        input.PopNextEvent();
    }
    
    return command;
}

MapController::Command MapController::HandleTouch(const Pht::TouchEvent& touch) {
    UpdateTouchingState(touch);
    
    mEngine.GetRenderer().SetProjectionMode(Pht::ProjectionMode::Orthographic);

    auto& pins {mScene.GetPins()};
    
    for (const auto& pin: pins) {
        if (!pin->IsClickable()) {
            continue;
        }
        
        switch (pin->GetButton().OnTouch(touch, identityMatrix)) {
            case Pht::Button::Result::Down:
                pin->SetIsSelected(true);
                return Command::None;
            case Pht::Button::Result::MoveInside:
                return Command::None;
            case Pht::Button::Result::MoveOutside:
                pin->SetIsSelected(false);
                StartPan(touch);
                break;
            case Pht::Button::Result::UpInside:
                pin->SetIsSelected(false);
                return HandleLevelClick(pin->GetLevel());
            default:
                pin->SetIsSelected(false);
                break;
        }
    }
    
    Pan(touch);
    
    return Command::None;
}

MapController::Command MapController::HandleLevelClick(int level) {
    if (mUserData.GetLifeManager().GetNumLives() == 0) {
        mState = State::NoLivesDialog;
        mMapViewControllers.SetActiveController(MapViewControllers::NoLivesDialog);
        mMapViewControllers.GetNoLivesDialogController().Reset(SlidingMenuAnimation::UpdateFade::Yes,
                                                               true);
        return Command::None;
    }
    
    return Command {Command::StartGame, level};
}

void MapController::UpdateTouchingState(const Pht::TouchEvent& touch) {
    switch (touch.mState) {
        case Pht::TouchState::Begin:
        case Pht::TouchState::Ongoing:
            mIsTouching = true;
            break;
        default:
            mIsTouching = false;
            break;
    }
}

void MapController::Pan(const Pht::TouchEvent& touch) {
    switch (touch.mState) {
        case Pht::TouchState::Begin:
            StartPan(touch);
            break;
        case Pht::TouchState::Ongoing: {
            auto scaleFactor {
                mEngine.GetRenderer().GetOrthographicFrustumSize().y /
                mEngine.GetInput().GetScreenInputSize().y
            };
            
            auto translation = touch.mLocation - mTouchLocationAtPanBegin;
            
            Pht::Vec3 newCameraPosition {
                mCameraPositionAtPanBegin.x - translation.x * scaleFactor,
                mCameraPositionAtPanBegin.y + translation.y * scaleFactor,
                mCameraPositionAtPanBegin.z
            };
            
            mCameraVelocity = (newCameraPosition - mScene.GetCameraPosition()) /
                              mEngine.GetLastFrameSeconds();
            
            mScene.SetCameraPosition(newCameraPosition);
            break;
        }
        default:
            break;
    }
}

void MapController::StartPan(const Pht::TouchEvent& touch) {
    mTouchLocationAtPanBegin = touch.mLocation;
    mCameraPositionAtPanBegin = mScene.GetCameraPosition();
}

void MapController::UpdateCamera() {
    auto cameraPosition {mScene.GetCameraPosition()};
    cameraPosition += mCameraVelocity * mEngine.GetLastFrameSeconds();
    mScene.SetCameraPosition(cameraPosition);
    mCameraVelocity *= cameraDeaccelerationFactor;
}

void MapController::GoToSettingsMenuState() {
    mMapViewControllers.SetActiveController(MapViewControllers::SettingsMenu);
    mMapViewControllers.GetSettingsMenuController().Reset(SlidingMenuAnimation::UpdateFade::Yes);
    mState = State::SettingsMenu;
}
