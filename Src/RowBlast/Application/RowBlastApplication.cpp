#include "RowBlastApplication.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "IAudio.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "UiLayer.hpp"
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fadeDuration {0.22f};
    constexpr auto musicFadeOutDuration {0.22f};
    constexpr auto firstSceneFadeInDuration {1.0f};
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
    } {

    engine.GetInput().SetUseGestureRecognizers(false);
    
    SetUpRenderer();
    SetUpAudio();
    
    if (mAcceptTermsController.IsTermsAccepted()) {
        StartTitleScene();
    } else {
        StartAcceptTermsScene();
    }
    
    mFadeEffect.GetSceneObject().SetLayer(GlobalLayer::sceneSwitchFadeEffect);
    InsertFadeEffectInActiveScene();
    mFadeEffect.StartInMidFade();
    
    auto currentLevelId {mUserServices.GetProgressService().GetCurrentLevel()};
    mMapController.GetScene().SetWorldId(mUniverse.CalcWorldId(currentLevelId));
}

void RowBlastApplication::SetUpRenderer() {
    auto& renderer {mEngine.GetRenderer()};
    renderer.DisableShader(Pht::ShaderType::PixelLighting);
    renderer.DisableShader(Pht::ShaderType::PointParticle);
}

void RowBlastApplication::SetUpAudio() {
    auto& audio {mEngine.GetAudio()};
    auto& settingsService {mUserServices.GetSettingsService()};
    
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
    auto command {mAcceptTermsController.Update()};
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case AcceptTermsController::Command::None:
                break;
            case AcceptTermsController::Command::ViewTermsOfService:
            case AcceptTermsController::Command::ViewPrivacyPolicy:
            case AcceptTermsController::Command::Accept:
                mFadeEffect.SetDuration(fadeDuration);
                mFadeEffect.Start();
                mEngine.GetInput().DisableInput();
                mNextState = State::TitleScene;
                break;
        }
    }
}

void RowBlastApplication::UpdateTitleScene() {
    auto command {mTitleController.Update()};
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case TitleController::Command::None:
                break;
            case TitleController::Command::GoToMap:
                mFadeEffect.SetDuration(fadeDuration);
                BeginFadingToMap(MapInitialState::Map);
                break;
        }
    }
}

void RowBlastApplication::UpdateMapScene() {
    auto command {mMapController.Update()};
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command.GetKind()) {
            case MapController::Command::None:
                break;
            case MapController::Command::StartGame:
                BeginFadingToGame(command.GetLevel());
                break;
            case MapController::Command::StartMap:
                BeginFadingToMap(MapInitialState::Map);
                break;
        }
    }
}

void RowBlastApplication::UpdateGameScene() {
    auto command {mGameController.Update()};
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case GameController::Command::None:
                break;
            case GameController::Command::GoToMap:
                mLevelToStart = mUserServices.GetProgressService().GetCurrentLevel() + 1;
                if (mUserServices.GetProgressService().ProgressedAtPreviousGameRound()) {
                    BeginFadingToMap(MapInitialState::UfoAnimation);
                    mMapController.SetStartLevelDialogOnAnimationFinished(false);
                } else {
                    BeginFadingToMap(MapInitialState::Map);
                }
                break;
            case GameController::Command::RestartLevel:
                BeginFadingToMap(MapInitialState::LevelGoalDialog);
                break;
            case GameController::Command::GoToNextLevel:
                mLevelToStart = mUserServices.GetProgressService().GetCurrentLevel() + 1;
                if (mUserServices.GetProgressService().ProgressedAtPreviousGameRound()) {
                    BeginFadingToMap(MapInitialState::UfoAnimation);
                    mMapController.SetStartLevelDialogOnAnimationFinished(true);
                } else {
                    BeginFadingToMap(MapInitialState::LevelGoalDialog);
                }
                break;
        }
    }
}

void RowBlastApplication::HandleTransitions() {
    if (mFadeEffect.Update(mEngine.GetLastFrameSeconds()) == Pht::FadeEffect::State::Transition) {
        switch (mNextState) {
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

void RowBlastApplication::InsertFadeEffectInActiveScene() {
    auto* scene {mEngine.GetSceneManager().GetActiveScene()};
    scene->GetRoot().AddChild(mFadeEffect.GetSceneObject());
}

void RowBlastApplication::BeginFadingToMap(MapInitialState mapInitialState) {
    mMapInitialState = mapInitialState;
    mFadeEffect.Start();
    mNextState = State::MapScene;
    mEngine.GetInput().DisableInput();
    
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
    
    switch (mMapInitialState) {
        case MapInitialState::Map:
            break;
        case MapInitialState::LevelGoalDialog:
            mMapController.GetScene().SetCameraAtLevel(mLevelToStart);
            mMapController.GoToStartLevelStateLevelGoalDialog(mLevelToStart);
            break;
        case MapInitialState::UfoAnimation:
            mMapController.GoToUfoAnimationState(mLevelToStart);
            break;
    }
}

void RowBlastApplication::StartGameScene() {
    mState = State::GameScene;
    mGameController.StartLevel(mLevelToStart);
}
