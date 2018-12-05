#include "RowBlastApplication.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fadeDuration {0.22f};
    constexpr auto titleFadeInDuration {1.0f};
}

std::unique_ptr<Pht::IApplication> CreateApplication(Pht::IEngine& engine) {
    return std::make_unique<RowBlastApplication>(engine);
}

RowBlastApplication::RowBlastApplication(Pht::IEngine& engine) :
    mEngine {engine},
    mCommonResources {engine},
    mSettings {},
    mUserServices {engine},
    mUniverse {},
    mTitleController {engine, mCommonResources, mUserServices, mUniverse},
    mGameController {engine, mCommonResources, mUserServices, mSettings},
    mMapController {
        engine,
        mCommonResources,
        mUserServices,
        mSettings,
        mUniverse,
        mGameController.GetLevelResources(),
        mGameController.GetPieceResources()
    },
    mFadeEffect {engine.GetSceneManager(), engine.GetRenderer(), titleFadeInDuration, 1.0f, 0.0f} {

    engine.GetInput().SetUseGestureRecognizers(false);
    
    auto& renderer {engine.GetRenderer()};
    renderer.DisableShader(Pht::ShaderType::PixelLighting);
    renderer.DisableShader(Pht::ShaderType::PointParticle);
    
    mFadeEffect.GetSceneObject().SetLayer(GlobalLayer::sceneSwitchFadeEffect);
    InsertFadeEffectInActiveScene();
    mFadeEffect.StartInMidFade();
    
    auto currentLevelId {mUserServices.GetProgressService().GetCurrentLevel()};
    mMapController.GetScene().SetWorldId(mUniverse.CalcWorldId(currentLevelId));
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

void RowBlastApplication::UpdateTitleScene() {
    auto command {mTitleController.Update()};
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case TitleController::Command::None:
                break;
            case TitleController::Command::GoToMap:
                mFadeEffect.SetDuration(fadeDuration);
                BeginFadeToMap(MapInitialState::Map);
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
                BeginFadeToGame(command.GetLevel());
                break;
            case MapController::Command::StartMap:
                BeginFadeToMap(MapInitialState::Map);
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
                    BeginFadeToMap(MapInitialState::UfoAnimation);
                    mMapController.SetStartLevelDialogOnAnimationFinished(false);
                } else {
                    BeginFadeToMap(MapInitialState::Map);
                }
                break;
            case GameController::Command::RestartLevel:
                BeginFadeToMap(MapInitialState::LevelGoalDialog);
                break;
            case GameController::Command::GoToNextLevel:
                mLevelToStart = mUserServices.GetProgressService().GetCurrentLevel() + 1;
                if (mUserServices.GetProgressService().ProgressedAtPreviousGameRound()) {
                    BeginFadeToMap(MapInitialState::UfoAnimation);
                    mMapController.SetStartLevelDialogOnAnimationFinished(true);
                } else {
                    BeginFadeToMap(MapInitialState::LevelGoalDialog);
                }
                break;
        }
    }
}

void RowBlastApplication::HandleTransitions() {
    if (mFadeEffect.Update(mEngine.GetLastFrameSeconds()) == Pht::FadeEffect::State::Transition) {
        switch (mNextState) {
            case State::MapScene:
                StartMap();
                break;
            case State::GameScene:
                StartGame();
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

void RowBlastApplication::BeginFadeToMap(MapInitialState mapInitialState) {
    mMapInitialState = mapInitialState;
    mFadeEffect.Start();
    mNextState = State::MapScene;
    mEngine.GetInput().DisableInput();
}

void RowBlastApplication::BeginFadeToGame(int level) {
    mLevelToStart = level;
    mFadeEffect.Start();
    mNextState = State::GameScene;
    mEngine.GetInput().DisableInput();
}

void RowBlastApplication::StartMap() {
    mState = State::MapScene;
    mEngine.GetInput().EnableInput();
    mMapController.Init();
    
    switch (mMapInitialState) {
        case MapInitialState::Map:
            break;
        case MapInitialState::LevelGoalDialog:
            mMapController.GetScene().SetCameraAtLevel(mLevelToStart);
            mMapController.GoToLevelGoalDialogState(mLevelToStart);
            break;
        case MapInitialState::UfoAnimation:
            mMapController.GoToUfoAnimationState(mLevelToStart);
            break;
    }
}

void RowBlastApplication::StartGame() {
    mState = State::GameScene;
    mGameController.StartLevel(mLevelToStart);
}
