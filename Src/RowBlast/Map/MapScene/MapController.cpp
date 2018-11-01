#include "MapController.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "InputEvent.hpp"
#include "MathUtils.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "SettingsMenuController.hpp"
#include "NoLivesDialogController.hpp"
#include "UserServices.hpp"
#include "LevelLoader.hpp"
#include "Universe.hpp"

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
                             UserServices& userServices,
                             Settings& settings,
                             const Universe& universe,
                             const LevelResources& levelResources,
                             PieceResources& pieceResources) :
    mEngine {engine},
    mUserServices {userServices},
    mLevelResources {levelResources},
    mUniverse {universe},
    mScene {engine, commonResources, userServices, universe},
    mUfo {engine, commonResources, 1.0f},
    mUfoAnimation {engine, mUfo},
    mMapViewControllers {engine, mScene, commonResources, userServices, settings, pieceResources} {}

void MapController::Init() {
    mScene.Init();
    mUfo.Init(mScene.GetUfoContainer());
    mUfoAnimation.Init();
    mMapViewControllers.Init();
    
    mState = State::Map;
    mCameraXVelocity = 0.0f;
    
    if (auto* currentPin {mScene.GetLevelPin(mUserServices.GetProgressService().GetProgress())}) {
        mUfo.SetPosition(currentPin->GetUfoPosition());
    } else {
        mUfo.Hide();
    }
    
    mEngine.GetSceneManager().InitRenderer();
}

MapController::Command MapController::Update() {
    auto command {Command{Command::None}};
    
    UpdateUfoAnimation();
    
    switch (mState) {
        case State::Map:
        case State::UfoAnimation:
            command = UpdateMap();
            break;
        case State::LevelGoalDialog:
            command = UpdateLevelGoalDialog();
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

MapController::Command MapController::UpdateMap() {
    auto command {HandleInput()};
    
    if (!mIsTouching) {
        UpdateCamera();
    }
    
    return command;
}

void MapController::UpdateUfoAnimation() {
    switch (mUfoAnimation.Update()) {
        case UfoAnimation::State::Active:
            if (mCameraShouldFollowUfo) {
                mScene.SetCameraXPosition(mUfo.GetPosition().x);
            }
            break;
        case UfoAnimation::State::Finished:
            if (mCameraShouldFollowUfo) {
                mScene.SetCameraXPosition(mUfo.GetPosition().x);
            }
            if (mState == State::UfoAnimation) {
                if (mStartLevelDialogOnAnimationFinished) {
                    GoToLevelGoalDialogState(mLevelToStart);
                }
                if (mHideUfoOnAnimationFinished) {
                    mUfo.Hide();
                }
            }
            break;
        default:
            break;
    }
}

MapController::Command MapController::UpdateLevelGoalDialog() {
    auto command {Command{Command::None}};

    switch (mMapViewControllers.GetLevelGoalDialogController().Update()) {
        case LevelGoalDialogController::Result::None:
            break;
        case LevelGoalDialogController::Result::Play:
            command = Command {Command::StartGame, mLevelToStart};
            break;
        case LevelGoalDialogController::Result::Close:
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
            mUserServices.GetLifeService().RefillLives();
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
    auto command {Command{Command::None}};
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
                return command;
            case Pht::Button::Result::MoveInside:
                return command;
            case Pht::Button::Result::MoveOutside:
                pin->SetIsSelected(false);
                StartPan(touch);
                break;
            case Pht::Button::Result::UpInside:
                pin->SetIsSelected(false);
                return HandlePinClick(*pin);
            default:
                pin->SetIsSelected(false);
                break;
        }
    }
    
    if (!mUfoAnimation.IsActive()) {
        Pan(touch);
    }

    return command;
}

MapController::Command MapController::HandlePinClick(const MapPin& pin) {
    auto command {Command{Command::None}};
    auto& mapPlace {pin.GetPlace()};
    
    switch (mapPlace.GetKind()) {
        case MapPlace::Kind::MapLevel:
            if (mUserServices.GetLifeService().GetNumLives() > 0) {
                GoToLevelGoalDialogState(pin.GetLevel());
            } else {
                mState = State::NoLivesDialog;
                mMapViewControllers.SetActiveController(MapViewControllers::NoLivesDialog);
                mMapViewControllers.GetNoLivesDialogController().Init(true);
            }
            break;
        case MapPlace::Kind::Portal: {
            auto& portal {mapPlace.GetPortal()};
            mScene.SetWorldId(portal.mDestinationWorldId);
            mScene.SetClickedPortalNextLevelId(portal.mNextLevelId);
            command = Command {Command::StartMap};
            break;
        }
    }
    
    return command;
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

void MapController::GoToUfoAnimationState(int levelToStart) {
    mState = State::UfoAnimation;
    mLevelToStart = levelToStart;
    
    auto nextLevel {mUserServices.GetProgressService().GetProgress()};
    auto* nextPin {mScene.GetPin(nextLevel)};
    auto* currentPin {mScene.GetLevelPin(nextLevel - 1)};
    
    if (nextPin && currentPin) {
        mUfo.Show();
        mUfo.SetPosition(currentPin->GetUfoPosition());
        mUfoAnimation.Start(nextPin->GetUfoPosition());
        mScene.SetCameraXPosition(mUfo.GetPosition().x);
        mCameraShouldFollowUfo = true;
    }
    
    if (nextPin && nextPin->GetPlace().GetKind() == MapPlace::Kind::Portal) {
        mHideUfoOnAnimationFinished = true;
        mStartLevelDialogOnAnimationFinished = false;
        mUfoAnimation.StartHighSpeed(nextPin->GetUfoPosition());
        mScene.SetCameraBetweenLevels(nextLevel - 1, nextLevel);
        mCameraShouldFollowUfo = false;
    } else {
        mHideUfoOnAnimationFinished = false;
    }
}

void MapController::GoToLevelGoalDialogState(int levelToStart) {
    if (mScene.GetWorldId() != mUniverse.CalcWorldId(levelToStart)) {
        return;
    }

    mState = State::LevelGoalDialog;
    mLevelToStart = levelToStart;
    mMapViewControllers.SetActiveController(MapViewControllers::LevelGoalDialog);

    auto levelInfo {LevelLoader::LoadInfo(levelToStart, mLevelResources)};
    mMapViewControllers.GetLevelGoalDialogController().Init(*levelInfo);
}

void MapController::GoToSettingsMenuState() {
    mMapViewControllers.SetActiveController(MapViewControllers::SettingsMenu);
    mMapViewControllers.GetSettingsMenuController().Init(SlidingMenuAnimation::UpdateFade::Yes,
                                                         true);
    mState = State::SettingsMenu;
}
