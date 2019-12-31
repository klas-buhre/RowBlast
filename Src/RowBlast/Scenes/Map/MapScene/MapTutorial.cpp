#include "MapTutorial.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IAnalytics.hpp"
#include "AnalyticsEvent.hpp"

// Game includes.
#include "MapScene.hpp"
#include "UserServices.hpp"

using namespace RowBlast;

MapTutorial::MapTutorial(Pht::IEngine& engine, MapScene& scene, const UserServices& userServices) :
    mEngine {engine},
    mScene {scene},
    mUserServices {userServices},
    mHandAnimation {engine, 0.7f, true} {}

void MapTutorial::Init(int worldId) {
    auto progress = mUserServices.GetProgressService().GetProgress();

    if (worldId == 1 && progress == 1) {
        if (mState != State::StartLevel1StepStart && mState != State::StartLevel1StepComplete) {
            mState = State::StartLevel1StepStart;
            SendAnalyticsEvent("StartLevel1Step", "Start");
        }
    } else if (worldId == 1 && progress == 19) {
        if (mState != State::GoToWorld2StepStart && mState != State::GoToWorld2StepComplete) {
            mState = State::GoToWorld2StepStart;
            SendAnalyticsEvent("GoToWorld2Step", "Start");
        }
    } else {
        mState = State::Inactive;
        return;
    }
    
    mHandAnimation.Init(mScene.GetTutorialContainer());
    
    auto* pin = mScene.GetPin(progress);
    assert(pin);
    
    auto handPosition = pin->GetPosition() + Pht::Vec3{0.76f, 0.25f, 1.0f};
    mHandAnimation.Start(handPosition, 90.0f);
}

void MapTutorial::Update() {
    switch (mState) {
        case State::Inactive:
            break;
        case State::StartLevel1StepStart:
        case State::StartLevel1StepComplete:
        case State::GoToWorld2StepStart:
        case State::GoToWorld2StepComplete:
            mHandAnimation.Update();
            break;
    }
}

void MapTutorial::OnLevelClick() {
    switch (mState) {
        case State::StartLevel1StepStart:
            mState = State::StartLevel1StepComplete;
            SendAnalyticsEvent("StartLevel1Step", "Complete");
            break;
        default:
            break;
    }
}

void MapTutorial::OnPortalClick() {
    switch (mState) {
        case State::GoToWorld2StepStart:
            mState = State::GoToWorld2StepComplete;
            SendAnalyticsEvent("GoToWorld2Step", "Complete");
            break;
        default:
            break;
    }
}

void MapTutorial::SendAnalyticsEvent(const std::string& step, const std::string& status) {
    Pht::CustomAnalyticsEvent analyticsEvent {"MapTutorial:" + step + ":" + status};
    mEngine.GetAnalytics().AddEvent(analyticsEvent);
}
