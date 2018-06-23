#include "MapController.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "InputEvent.hpp"
#include "MathUtils.hpp"

// Game includes.
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "UserData.hpp"
#include "LevelLoader.hpp"

using namespace RowBlast;

namespace {
    constexpr auto cameraCutoffVelocity {0.1f};
    constexpr auto dampingCoefficient {5.0f};
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
                             UserData& userData,
                             Settings& settings,
                             const LevelResources& levelResources,
                             PieceResources& pieceResources) :
    mEngine {engine},
    mUserData {userData},
    mLevelResources {levelResources},
    mScene {engine, commonResources, userData},
    mAvatar {engine, mScene, commonResources},
    mAvatarAnimation {engine, mAvatar},
    mMapViewControllers {engine, mScene, commonResources, userData, settings, pieceResources} {}

void MapController::Init() {
    mScene.Init();
    mAvatar.Init();
    mAvatarAnimation.Init();
    mMapViewControllers.Init();
    
    mState = State::Map;
    mCameraXVelocity = 0.0f;
    
    auto& currentPin {*mScene.GetPins()[mUserData.GetProgressManager().GetProgress() - 1]};
    mAvatar.SetPosition(currentPin.GetPosition());
}

MapController::Command MapController::Update() {
    auto command {Command{Command::None}};
    
    UpdateAvatarAnimation();
    
    switch (mState) {
        case State::Map:
        case State::AvatarAnimation:
            UpdateMap();
            break;
        case State::LevelStartDialog:
            command = UpdateLevelStartDialog();
            break;
        case State::NoLivesDialog:
            UpdateNoLivesDialog();
            break;
        case State::SettingsMenu:
            UpdateSettingsMenu();
            break;
    }

    mScene.Update();
    
    return command;
}

void MapController::UpdateMap() {
    HandleInput();
    
    if (!mIsTouching) {
        UpdateCamera();
    }
}

void MapController::UpdateAvatarAnimation() {
    if (mAvatarAnimation.Update() == AvatarAnimation::State::Finished) {
        if (mState == State::AvatarAnimation && mStartLevelDialogOnAnimationFinished) {
            GoToLevelStartDialogState(mLevelToStart);
        }
    }
}

MapController::Command MapController::UpdateLevelStartDialog() {
    auto command {Command{Command::None}};

    switch (mMapViewControllers.GetLevelStartDialogController().Update()) {
        case LevelStartDialogController::Result::None:
            break;
        case LevelStartDialogController::Result::Play:
            command = Command {Command::StartGame, mLevelToStart};
            break;
        case LevelStartDialogController::Result::Close:
            mState = State::Map;
            mMapViewControllers.SetActiveController(MapViewControllers::SettingsButton);
            break;
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

void MapController::HandleInput() {
    auto& input {mEngine.GetInput()};
    
    while (input.HasEvents()) {
        auto& event {input.GetNextEvent()};
        switch (event.GetKind()) {
            case Pht::InputKind::Touch: {
                auto& touchEvent {event.GetTouchEvent()};
                switch (mMapViewControllers.GetSettingsButtonController().OnTouch(touchEvent)) {
                    case SettingsButtonController::Result::None:
                        HandleTouch(touchEvent);
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
}

void MapController::HandleTouch(const Pht::TouchEvent& touch) {
    UpdateTouchingState(touch);
    
    mEngine.GetRenderer().SetProjectionMode(Pht::ProjectionMode::Perspective);

    auto& pins {mScene.GetPins()};
    
    for (const auto& pin: pins) {
        if (!pin->IsClickable()) {
            continue;
        }
        
        switch (pin->GetButton().OnTouch(touch)) {
            case Pht::Button::Result::Down:
                pin->SetIsSelected(true);
                return;
            case Pht::Button::Result::MoveInside:
                return;
            case Pht::Button::Result::MoveOutside:
                pin->SetIsSelected(false);
                StartPan(touch);
                break;
            case Pht::Button::Result::UpInside:
                pin->SetIsSelected(false);
                HandlePinClick(*pin);
                return;
            default:
                pin->SetIsSelected(false);
                break;
        }
    }
    
    Pan(touch);
}

void MapController::HandlePinClick(const MapPin& pin) {
    auto& mapPlace {pin.GetPlace()};
    
    switch (mapPlace.GetKind()) {
        case MapPlace::Kind::MapLevel:
            if (mUserData.GetLifeManager().GetNumLives() > 0) {
                GoToLevelStartDialogState(pin.GetLevel());
            } else {
                mState = State::NoLivesDialog;
                mMapViewControllers.SetActiveController(MapViewControllers::NoLivesDialog);
                mMapViewControllers.GetNoLivesDialogController().Init(true);
            }
            break;
        case MapPlace::Kind::Portal:
            break;
    }
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
            auto translation = touch.mLocation - mTouchLocationAtPanBegin;
            auto newCameraXPosition {mCameraXPositionAtPanBegin - translation.x * 0.024f};
            
            mCameraXVelocity = (newCameraXPosition - mScene.GetCameraXPosition()) /
                               mEngine.GetLastFrameSeconds();
            
            mScene.SetCameraXPosition(newCameraXPosition);
            break;
        }
        default:
            break;
    }
}

void MapController::StartPan(const Pht::TouchEvent& touch) {
    mTouchLocationAtPanBegin = touch.mLocation;
    mCameraXPositionAtPanBegin = mScene.GetCameraXPosition();
}

void MapController::UpdateCamera() {
    if (mCameraXVelocity == 0.0f) {
        return;
    }
    
    auto cameraXPosition {mScene.GetCameraXPosition()};
    auto deacceleration {dampingCoefficient * mCameraXVelocity};
    auto dt {mEngine.GetLastFrameSeconds()};
    auto previousVelocity {mCameraXVelocity};
    mCameraXVelocity -= deacceleration * dt;
    
    if (std::fabs(mCameraXVelocity) < cameraCutoffVelocity ||
        Pht::Sign(mCameraXVelocity) != Pht::Sign(previousVelocity)) {

        mCameraXVelocity = 0.0f;
    }

    cameraXPosition += mCameraXVelocity * dt;
    mScene.SetCameraXPosition(cameraXPosition);
}

void MapController::GoToAvatarAnimationState(int levelToStart) {
    mState = State::AvatarAnimation;
    mLevelToStart = levelToStart;
    
    auto nextLevel {mUserData.GetProgressManager().GetProgress()};
    auto& nextPin {*mScene.GetPins()[nextLevel - 1]};
    auto& currentPin {*mScene.GetPins()[nextLevel - 2]};
    
    mAvatar.SetPosition(currentPin.GetPosition());
    mAvatarAnimation.Start(nextPin.GetPosition());
}

void MapController::GoToLevelStartDialogState(int levelToStart) {
    mState = State::LevelStartDialog;
    mLevelToStart = levelToStart;
    mMapViewControllers.SetActiveController(MapViewControllers::LevelStartDialog);

    auto levelInfo {LevelLoader::LoadInfo(levelToStart, mLevelResources)};
    mMapViewControllers.GetLevelStartDialogController().Init(*levelInfo);
}

void MapController::GoToSettingsMenuState() {
    mMapViewControllers.SetActiveController(MapViewControllers::SettingsMenu);
    mMapViewControllers.GetSettingsMenuController().Init(SlidingMenuAnimation::UpdateFade::Yes);
    mState = State::SettingsMenu;
}
