#include "MapTutorial.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"

// Game includes.
#include "MapScene.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

MapTutorial::MapTutorial(Pht::IEngine& engine, MapScene& scene, const UserServices& userServices) :
    mScene {scene},
    mUserServices {userServices},
    mHandAnimation {engine} {}

void MapTutorial::Init(int worldId) {
    auto progress {mUserServices.GetProgressService().GetProgress()};
    
    if (worldId == 1 && (progress == 1 || progress == 19)) {
        mState = State::Active;
    } else {
        mState = State::Inactive;
        return;
    }
    
    mHandAnimation.Init(mScene.GetScene(), mScene.GetTutorialContainer(), 0.43f, false);
    
    auto* pin {mScene.GetPin(progress)};
    assert(pin);
    
    auto handPosition {pin->GetPosition() + Pht::Vec3{0.76f, 0.25f, 1.0f}};
    mHandAnimation.Start(handPosition, 90.0f);
}

void MapTutorial::Update() {
    switch (mState) {
        case State::Inactive:
            break;
        case State::Active:
            mHandAnimation.Update();
            break;
    }
}
