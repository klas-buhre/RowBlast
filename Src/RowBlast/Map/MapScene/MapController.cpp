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
                             const Universe& universe,
                             const LevelResources& levelResources,
                             PieceResources& pieceResources) :
    mEngine {engine},
    mUserServices {userServices},
    mLevelResources {levelResources},
    mUniverse {universe},
    mScene {engine, commonResources, userServices, universe},
    mTutorial {engine, mScene, userServices},
    mUfo {engine, commonResources, 1.17f},
    mUfoAnimation {engine, mUfo},
    mMapViewControllers {engine, mScene, commonResources, userServices, pieceResources},
    mStoreController {
        engine,
        commonResources,
        userServices,
        mScene,
        StoreController::SceneId::Map
    } {}

void MapController::Init() {
    mScene.Init();
    mTutorial.Init(mScene.GetWorldId());
    mUfo.Init(mScene.GetUfoContainer());
    mUfoAnimation.Init();
    mMapViewControllers.Init(mStoreController.GetFadeEffect());
    mStoreController.Init(mScene.GetUiViewsContainer());
    
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
        case State::StartLevel:
            command = UpdateInStartLevelState();
            break;
        case State::LivesDialog:
            UpdateLivesDialog();
            break;
        case State::AddLives:
            UpdateInAddLivesState();
            break;
        case State::SettingsMenu:
            UpdateSettingsMenu();
            break;
        case State::AddCoinsStore:
            UpdateInAddCoinsStoreState();
            break;
    }

    mScene.Update();
    mTutorial.Update();
    
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
                    GoToStartLevelStateLevelGoalDialog(mLevelToStart);
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

MapController::Command MapController::UpdateInStartLevelState() {
    auto command {Command{Command::None}};

    switch (mStartLevelState) {
        case StartLevelState::LevelGoalDialog:
            command = UpdateInStartLevelStateLevelGoalDialog();
            break;
        case StartLevelState::NoLivesDialog:
            UpdateInStartLevelStateNoLivesDialog();
            break;
        case StartLevelState::Store:
            UpdateInStartLevelStateStore();
            break;
    }
    
    return command;
}

MapController::Command MapController::UpdateInStartLevelStateLevelGoalDialog() {
    auto command {Command{Command::None}};

    switch (mMapViewControllers.GetLevelGoalDialogController().Update()) {
        case LevelGoalDialogController::Result::None:
            break;
        case LevelGoalDialogController::Result::Play:
            command = Command {Command::StartGame, mLevelToStart};
            break;
        case LevelGoalDialogController::Result::Close:
            GoToMapState();
            break;
    }
    
    return command;
}

void MapController::UpdateInStartLevelStateNoLivesDialog() {
    switch (mMapViewControllers.GetNoLivesDialogController().Update()) {
        case NoLivesDialogController::Result::None:
            break;
        case NoLivesDialogController::Result::RefillLives:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::refillLivesPriceInCoins)) {
                RefillLives();
                GoToStartLevelStateLevelGoalDialog(mLevelToStart);
            } else {
                GoToStartLevelStateStore();
            }
            break;
        case NoLivesDialogController::Result::Close:
            GoToMapState();
            break;
    }
}

void MapController::UpdateInStartLevelStateStore() {
    switch (mStoreController.Update()) {
        case StoreController::Result::None:
            break;
        case StoreController::Result::Done:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::refillLivesPriceInCoins)) {
                RefillLives();
                GoToStartLevelStateLevelGoalDialog(mLevelToStart);
            } else {
                GoToMapState();
            }
            break;
    }
}

void MapController::UpdateInAddLivesState() {
    switch (mAddLivesState) {
        case AddLivesState::NoLivesDialog:
            UpdateInAddLivesStateNoLivesDialog();
            break;
        case AddLivesState::Store:
            UpdateInAddLivesStateStore();
            break;
    }
}

void MapController::UpdateInAddLivesStateNoLivesDialog() {
    switch (mMapViewControllers.GetNoLivesDialogController().Update()) {
        case NoLivesDialogController::Result::None:
            break;
        case NoLivesDialogController::Result::RefillLives:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::refillLivesPriceInCoins)) {
                RefillLives();
                GoToMapState();
            } else {
                GoToAddLivesStateStore();
            }
            break;
        case NoLivesDialogController::Result::Close:
            GoToMapState();
            break;
    }
}

void MapController::UpdateInAddLivesStateStore() {
    switch (mStoreController.Update()) {
        case StoreController::Result::None:
            break;
        case StoreController::Result::Done:
            if (mUserServices.GetPurchasingService().CanAfford(PurchasingService::refillLivesPriceInCoins)) {
                RefillLives();
            }
            GoToMapState();
            break;
    }
}

void MapController::RefillLives() {
    mUserServices.GetPurchasingService().WithdrawCoins(PurchasingService::refillLivesPriceInCoins);
    mUserServices.GetLifeService().RefillLives();
}

void MapController::UpdateLivesDialog() {
    switch (mMapViewControllers.GetLivesDialogController().Update()) {
        case LivesDialogController::Result::None:
            break;
        case LivesDialogController::Result::Close:
            GoToMapState();
            break;
    }
}

void MapController::UpdateSettingsMenu() {
    switch (mMapViewControllers.GetSettingsMenuController().Update()) {
        case SettingsMenuController::Result::None:
            break;
        case SettingsMenuController::Result::GoBack:
            GoToMapState();
            break;
    }
}

void MapController::UpdateInAddCoinsStoreState() {
    switch (mStoreController.Update()) {
        case StoreController::Result::None:
            break;
        case StoreController::Result::Done:
            GoToMapState();
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
                switch (mMapViewControllers.GetMapHudController().OnTouch(touchEvent)) {
                    case MapHudController::Result::None:
                        command = HandleTouch(touchEvent);
                        break;
                    case MapHudController::Result::ClickedSettingsButton:
                        GoToSettingsMenuState();
                        break;
                    case MapHudController::Result::ClickedCoinsButton:
                        GoToAddCoinsStoreState();
                        break;
                    case MapHudController::Result::ClickedLivesButton:
                        HandleLivesButtonClick();
                        break;
                    case MapHudController::Result::TouchStartedOverButton:
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
    
    auto& renderer {mEngine.GetRenderer()};
    renderer.SetProjectionMode(Pht::ProjectionMode::Perspective);
    renderer.SetHudMode(false);

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
                GoToStartLevelStateLevelGoalDialog(pin.GetLevel());
            } else {
                GoToStartLevelStateNoLivesDialog(pin.GetLevel());
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

void MapController::GoToStartLevelStateLevelGoalDialog(int levelToStart) {
    if (mScene.GetWorldId() != mUniverse.CalcWorldId(levelToStart)) {
        return;
    }

    mState = State::StartLevel;
    mStartLevelState = StartLevelState::LevelGoalDialog;
    mLevelToStart = levelToStart;
    mMapViewControllers.SetActiveController(MapViewControllers::LevelGoalDialog);

    auto levelInfo {LevelLoader::LoadInfo(levelToStart, mLevelResources)};
    mMapViewControllers.GetLevelGoalDialogController().SetUp(*levelInfo);
}

void MapController::GoToStartLevelStateNoLivesDialog(int levelToStart) {
    mState = State::StartLevel;
    mStartLevelState = StartLevelState::NoLivesDialog;
    mLevelToStart = levelToStart;
    mMapViewControllers.SetActiveController(MapViewControllers::NoLivesDialog);
    mMapViewControllers.GetNoLivesDialogController().SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                                           NoLivesDialogController::ShouldSlideOut::Yes,
                                                           NoLivesDialogController::ShouldSlideOut::Yes);
}

void MapController::GoToStartLevelStateStore() {
    mState = State::StartLevel;
    mStartLevelState = StartLevelState::Store;
    mMapViewControllers.SetActiveController(MapViewControllers::None);
    mStoreController.StartStore(StoreController::TriggerProduct::Lives,
                                SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::UpdateFade::Yes,
                                SlidingMenuAnimation::UpdateFade::Yes,
                                PurchaseSuccessfulDialogController::ShouldSlideOut::Yes);
}

void MapController::HandleLivesButtonClick() {
    if (mUserServices.GetLifeService().GetNumLives() == 0) {
        GoToAddLivesStateNoLivesDialog();
    } else {
        GoToLivesDailogState();
    }
}

void MapController::GoToLivesDailogState() {
    mState = State::LivesDialog;
    mMapViewControllers.SetActiveController(MapViewControllers::LivesDialog);
    mMapViewControllers.GetLivesDialogController().SetUp();
}

void MapController::GoToAddLivesStateNoLivesDialog() {
    mState = State::AddLives;
    mAddLivesState = AddLivesState::NoLivesDialog;
    mMapViewControllers.SetActiveController(MapViewControllers::NoLivesDialog);
    mMapViewControllers.GetNoLivesDialogController().SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                                           NoLivesDialogController::ShouldSlideOut::Yes,
                                                           NoLivesDialogController::ShouldSlideOut::Yes);
}

void MapController::GoToAddLivesStateStore() {
    mState = State::AddLives;
    mAddLivesState = AddLivesState::Store;
    mMapViewControllers.SetActiveController(MapViewControllers::None);
    mStoreController.StartStore(StoreController::TriggerProduct::Lives,
                                SlidingMenuAnimation::UpdateFade::No,
                                SlidingMenuAnimation::UpdateFade::Yes,
                                SlidingMenuAnimation::UpdateFade::Yes,
                                PurchaseSuccessfulDialogController::ShouldSlideOut::Yes);
}

void MapController::GoToSettingsMenuState() {
    mMapViewControllers.SetActiveController(MapViewControllers::SettingsMenu);
    mMapViewControllers.GetSettingsMenuController().SetUp(SlidingMenuAnimation::UpdateFade::Yes,
                                                          true);
    mState = State::SettingsMenu;
}

void MapController::GoToAddCoinsStoreState() {
    mState = State::AddCoinsStore;
    mMapViewControllers.SetActiveController(MapViewControllers::None);
    mStoreController.StartStore(StoreController::TriggerProduct::Coins,
                                SlidingMenuAnimation::UpdateFade::Yes,
                                SlidingMenuAnimation::UpdateFade::Yes,
                                SlidingMenuAnimation::UpdateFade::Yes,
                                PurchaseSuccessfulDialogController::ShouldSlideOut::Yes);
}

void MapController::GoToMapState() {
    mState = State::Map;
    mMapViewControllers.SetActiveController(MapViewControllers::MapHud);
}
