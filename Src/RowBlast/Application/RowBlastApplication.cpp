#include "RowBlastApplication.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "IAudio.hpp"
#include "ISceneManager.hpp"
#include "IAnalytics.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fadeDuration = 0.22f;
    constexpr auto portalFadeDuration = 0.45f;
    constexpr auto musicFadeOutDuration = 0.22f;
    constexpr auto firstSceneFadeInDuration = 1.0f;
}

std::unique_ptr<Pht::IApplication> CreateApplication(Pht::IEngine& engine) {
    return std::make_unique<RowBlastApplication>(engine);
}

RowBlastApplication::RowBlastApplication(Pht::IEngine& engine) :
    mEngine {engine},
    mCommonResources {engine},
    mUserServices {engine},
    mUniverse {},
    mAcceptTermsController {engine, mCommonResources},
    mDocumentViewerController {engine, mCommonResources},
    mTitleController {engine, mCommonResources, mUserServices, mUniverse},
    mGameController {engine, mCommonResources, mUserServices},
    mMapController {
        engine,
        mCommonResources,
        mUserServices,
        mUniverse,
        mGameController.GetLevelResources(),
        mGameController.GetPieceResources()
    },
    mFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        firstSceneFadeInDuration,
        1.0f,
        0.0f
    },
    mPortalFadeEffect {
        engine.GetSceneManager(),
        engine.GetRenderer(),
        portalFadeDuration,
        1.0f,
        0.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    } {

    engine.GetInput().SetUseGestureRecognizers(false);
    
    SetUpRenderer();
    SetUpAudio();
    
    if (mAcceptTermsController.IsTermsAccepted()) {
        mEngine.GetAnalytics().InitAnalytics();
        StartTitleScene();
    } else {
        StartAcceptTermsScene();
    }
    
    mFadeEffect.GetSceneObject().SetLayer(GlobalLayer::sceneSwitchFadeEffect);
    mPortalFadeEffect.GetSceneObject().SetLayer(GlobalLayer::sceneSwitchFadeEffect);
    InsertFadeEffectInActiveScene();
    mFadeEffect.StartInMidFade();
    
    auto currentLevelId = mUserServices.GetProgressService().GetCurrentLevel();
    mMapController.GetScene().SetWorldId(mUniverse.CalcWorldId(currentLevelId));
}

void RowBlastApplication::SetUpRenderer() {
    auto& renderer = mEngine.GetRenderer();
    renderer.DisableShader(Pht::ShaderId::PixelLighting);
    renderer.DisableShader(Pht::ShaderId::PointParticle);
}

void RowBlastApplication::SetUpAudio() {
    auto& audio = mEngine.GetAudio();
    auto& settingsService = mUserServices.GetSettingsService();

    if (settingsService.IsSoundEnabled()) {
        audio.EnableSound();
    } else {
        audio.DisableSound();
    }

    if (settingsService.IsMusicEnabled()) {
        audio.EnableMusic();
    } else {
        audio.DisableMusic();
    }
}

void RowBlastApplication::OnUpdate() {
    if (mState != State::GameScene) {
        mUserServices.Update();
    }
    
    UpdateScene();
    HandleTransitions();
}

void RowBlastApplication::UpdateScene() {
    switch (mState) {
        case State::AcceptTermsScene:
            UpdateAcceptTermsScene();
            break;
        case State::DocumentViewerScene:
            UpdateDocumentViewerScene();
            break;
        case State::TitleScene:
            UpdateTitleScene();
            break;
        case State::MapScene:
            UpdateMapScene();
            break;
        case State::GameScene:
            UpdateGameScene();
            break;
    }
}

void RowBlastApplication::UpdateAcceptTermsScene() {
    auto command = mAcceptTermsController.Update();
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case AcceptTermsController::Command::None:
                break;
            case AcceptTermsController::Command::ViewTermsOfService:
                BeginFadingToDocumentViewerScene(DocumentId::TermsOfService);
                break;
            case AcceptTermsController::Command::ViewPrivacyPolicy:
                BeginFadingToDocumentViewerScene(DocumentId::PrivacyPolicy);
                break;
            case AcceptTermsController::Command::Accept:
                mFadeEffect.SetDuration(fadeDuration);
                mFadeEffect.Start();
                mEngine.GetInput().DisableInput();
                mEngine.GetAnalytics().InitAnalytics();
                mNextState = State::TitleScene;
                break;
        }
    }
}

void RowBlastApplication::UpdateDocumentViewerScene() {
    auto command = mDocumentViewerController.Update();
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case DocumentViewerController::Command::None:
                break;
            case DocumentViewerController::Command::Close:
                if (mPreviousState == State::MapScene) {
                    BeginFadingToMap(MapInitialState::AboutMenu);
                } else {
                    mFadeEffect.Start();
                    mNextState = State::AcceptTermsScene;
                }
                break;
        }
    }
}

void RowBlastApplication::UpdateTitleScene() {
    auto command = mTitleController.Update();
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case TitleController::Command::None:
                break;
            case TitleController::Command::GoToMap:
                mFadeEffect.SetDuration(fadeDuration);
                if (mUserServices.GetProgressService().GetCurrentLevel() == 1) {
                    BeginFadingToGame(0);
                } else {
                    BeginFadingToMap(MapInitialState::Map);
                }
                break;
        }
    }
}

void RowBlastApplication::UpdateMapScene() {
    auto command = mMapController.Update();
    
    if (!mFadeEffect.IsFadingOut() && !mPortalFadeEffect.IsFadingOut()) {
        switch (command.GetKind()) {
            case MapController::Command::None:
                break;
            case MapController::Command::StartGame:
                BeginFadingToGame(command.GetLevel());
                break;
            case MapController::Command::StartMap:
                if (mUniverse.IsLastWorld(mMapController.GetScene().GetWorldId())) {
                    BeginFadingToMap(MapInitialState::NoMoreLevelsDialog);
                } else {
                    BeginFadingToMap(MapInitialState::Map);
                }
                break;
            case MapController::Command::ViewTermsOfService:
                BeginFadingToDocumentViewerScene(DocumentId::TermsOfService);
                break;
            case MapController::Command::ViewPrivacyPolicy:
                BeginFadingToDocumentViewerScene(DocumentId::PrivacyPolicy);
                break;
            case MapController::Command::ViewCredits:
                BeginFadingToDocumentViewerScene(DocumentId::Credits);
                break;
        }
    }
}

void RowBlastApplication::UpdateGameScene() {
    auto command = mGameController.Update();
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case GameController::Command::None:
                break;
            case GameController::Command::GoToMap: {
                auto& progressService = mUserServices.GetProgressService();
                mLevelToStart = progressService.GetCurrentLevel() + 1;
                if (progressService.GetProgress() == 1 && progressService.GetCurrentLevel() == 0) {
                    BeginFadingToMap(MapInitialState::FirstMapLevel);
                } else if (progressService.ProgressedAtPreviousGameRound()) {
                    BeginFadingToMap(MapInitialState::UfoAnimation);
                    mMapController.SetStartLevelDialogOnAnimationFinished(false);
                } else {
                    BeginFadingToMap(MapInitialState::Map);
                }
                break;
            }
            case GameController::Command::RestartLevel:
                BeginFadingToMap(MapInitialState::LevelGoalDialog);
                break;
            case GameController::Command::GoToNextLevel: {
                auto& progressService = mUserServices.GetProgressService();
                mLevelToStart = progressService.GetCurrentLevel() + 1;
                if (progressService.GetProgress() == 1 && progressService.GetCurrentLevel() == 0) {
                    BeginFadingToMap(MapInitialState::FirstMapLevel);
                } else if (progressService.ProgressedAtPreviousGameRound()) {
                    BeginFadingToMap(MapInitialState::UfoAnimation);
                    mMapController.SetStartLevelDialogOnAnimationFinished(true);
                } else {
                    BeginFadingToMap(MapInitialState::LevelGoalDialog);
                }
                break;
            }
        }
    }
}

void RowBlastApplication::HandleTransitions() {
    if (IsSceneTransition()) {
        switch (mNextState) {
            case State::AcceptTermsScene:
                StartAcceptTermsScene();
                break;
            case State::DocumentViewerScene:
                StartDocumentViewerScene();
                break;
            case State::TitleScene:
                StartTitleScene();
                break;
            case State::MapScene:
                StartMapScene();
                break;
            case State::GameScene:
                StartGameScene();
                break;
            default:
                break;
        }
        
        InsertFadeEffectInActiveScene();
    }
}

bool RowBlastApplication::IsSceneTransition() {
    if (mPortalFadeEffect.Update(mEngine.GetLastFrameSeconds()) ==
        Pht::FadeEffect::State::Transition) {
        
        return true;
    }
    
    return mFadeEffect.Update(mEngine.GetLastFrameSeconds()) == Pht::FadeEffect::State::Transition;
}

void RowBlastApplication::InsertFadeEffectInActiveScene() {
    auto* scene = mEngine.GetSceneManager().GetActiveScene();
    scene->GetRoot().AddChild(mFadeEffect.GetSceneObject());
}

void RowBlastApplication::BeginFadingToDocumentViewerScene(DocumentId documentToView) {
    mFadeEffect.SetDuration(fadeDuration);
    mFadeEffect.Start();
    mNextState = State::DocumentViewerScene;
    mDocumentToView = documentToView;
}

void RowBlastApplication::BeginFadingToMap(MapInitialState mapInitialState) {
    mMapInitialState = mapInitialState;
    mNextState = State::MapScene;
    mEngine.GetInput().DisableInput();
    
    if (mState == State::MapScene) {
        mPortalFadeEffect.Start();
    } else {
        mFadeEffect.Start();
    }
    
    if (mState == State::GameScene) {
        mEngine.GetAudio().FadeOutActiveTrack(musicFadeOutDuration);
    }
}

void RowBlastApplication::BeginFadingToGame(int level) {
    mLevelToStart = level;
    mFadeEffect.Start();
    mNextState = State::GameScene;
    mEngine.GetAudio().FadeOutActiveTrack(musicFadeOutDuration);
    mEngine.GetInput().DisableInput();
}

void RowBlastApplication::StartAcceptTermsScene() {
    mState = State::AcceptTermsScene;
    mAcceptTermsController.Init();
}

void RowBlastApplication::StartDocumentViewerScene() {
    mPreviousState = mState;
    mState = State::DocumentViewerScene;
    mDocumentViewerController.Init(mDocumentToView);
}

void RowBlastApplication::StartTitleScene() {
    mState = State::TitleScene;
    PlayMapMusicTrack(mEngine);
    mTitleController.Init();
}

void RowBlastApplication::StartMapScene() {
    if (mState == State::GameScene) {
        PlayMapMusicTrack(mEngine);
    }

    mState = State::MapScene;
    mEngine.GetInput().EnableInput();
    mMapController.Init();
    
    auto* scene = mEngine.GetSceneManager().GetActiveScene();
    scene->GetRoot().AddChild(mPortalFadeEffect.GetSceneObject());
    
    switch (mMapInitialState) {
        case MapInitialState::FirstMapLevel:
            mMapController.GetScene().SetCameraAtLevel(1);
            break;
        case MapInitialState::Map:
            break;
        case MapInitialState::LevelGoalDialog:
            mMapController.GetScene().SetCameraAtLevel(mLevelToStart);
            mMapController.GoToStartLevelStateLevelGoalDialog(mLevelToStart);
            break;
        case MapInitialState::UfoAnimation:
            mMapController.GoToUfoAnimationState(mLevelToStart);
            break;
        case MapInitialState::AboutMenu:
            mMapController.GoToAboutMenuState(SlidingMenuAnimation::UpdateFade::Yes);
            break;
        case MapInitialState::NoMoreLevelsDialog:
            mMapController.GoToNoMoreLevelsDialogState();
            break;
    }
}

void RowBlastApplication::StartGameScene() {
    mState = State::GameScene;
    mGameController.Init(mLevelToStart);
}
