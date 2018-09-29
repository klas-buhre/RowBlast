#include "TitleController.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "InputEvent.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec3 distantUfoPosition {40.0f, 220.0f, -600.0f};
    const Pht::Vec3 ufoPosition {0.25f, 1.95f, 11.0f};
}

TitleController::TitleController(Pht::IEngine& engine,
                                 const CommonResources& commonResources,
                                 const UserData& userData,
                                 const Universe& universe) :
    mEngine {engine},
    mScene {engine, commonResources, userData, universe},
    mUfo {engine, commonResources, 2.0f},
    mUfoAnimation {engine, mUfo} {
    
    mUfo.Init(mScene.GetUfoContainer());
    mUfoAnimation.Init();
    mUfo.SetPosition(distantUfoPosition);
    mUfoAnimation.StartWarpSpeed(ufoPosition);
}

TitleController::Command TitleController::Update() {
    auto command {Command::None};
    
    if (mEngine.GetInput().ConsumeWholeTouch()) {
        command = Command::GoToMap;
    }
    
    mScene.Update();
    mUfoAnimation.Update();
    
    return command;
}
