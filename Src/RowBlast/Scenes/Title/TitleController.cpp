#include "TitleController.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "IAudio.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "AudioResources.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 distantUfoPosition {40.0f, 220.0f, -600.0f};
    const Pht::Vec3 ufoPosition {0.25f, 1.95f, 11.0f};
}

TitleController::TitleController(Pht::IEngine& engine,
                                 const CommonResources& commonResources,
                                 const UserServices& userServices,
                                 const Universe& universe) :
    mEngine {engine},
    mScene {engine, commonResources, userServices, universe},
    mUfo {engine, commonResources, 2.0f},
    mUfoAnimation {engine, mUfo},
    mTitleAnimation {engine},
    mBeginTextAnimation {engine, commonResources} {}

void TitleController::Init() {
    mScene.Init();
    mUfo.Init(mScene.GetUfoContainer());
    mUfoAnimation.Init();
    mUfo.SetPosition(distantUfoPosition);
    mUfoAnimation.StartWarpSpeed(ufoPosition);
    mTitleAnimation.Init(mScene.GetScene(), mScene.GetUiContainer());
    mBeginTextAnimation.Init(mScene.GetScene(), mScene.GetUiContainer());
    
    mEngine.GetSceneManager().InitRenderer();
}

TitleController::Command TitleController::Update() {
    auto command {Command::None};
    
    if (mTitleAnimation.IsDone()) {
        if (!mBeginTextAnimation.IsActive()) {
            mBeginTextAnimation.Start();
        }
    
        if (mEngine.GetInput().ConsumeWholeTouch()) {
            command = Command::GoToMap;
            mEngine.GetAudio().PlaySound(static_cast<Pht::AudioResourceId>(SoundId::LeaveTitle));
        }
    }
    
    mScene.Update();
    mTitleAnimation.Update();
    mBeginTextAnimation.Update();
    mUfoAnimation.Update();
    
    return command;
}
