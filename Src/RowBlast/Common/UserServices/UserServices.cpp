#include "UserServices.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAnalytics.hpp"
#include "AnalyticsEvent.hpp"

using namespace RowBlast;

namespace {
    std::string ToControlTypeString(ControlType controlType) {
        switch (controlType) {
            case ControlType::Click:
                return "Click";
            case ControlType::Gesture:
                return "Gesture";
        }
    }
}

UserServices::UserServices(Pht::IEngine& engine) :
    mEngine {engine},
    mPurchasingService {engine} {}

void UserServices::Update() {
    mPurchasingService.Update();
    mLifeService.Update();
}

void UserServices::StartLevel(int levelId) {
    mProgressService.StartLevel(levelId);
    mLifeService.StartLevel();
    
    auto& analytics = mEngine.GetAnalytics();
    
    Pht::ProgressionAnalyticsEvent progressionAnalyticsEvent {
        Pht::ProgressionStatus::Start, std::to_string(levelId)
    };
    
    analytics.AddEvent(progressionAnalyticsEvent);
    
    Pht::CustomAnalyticsEvent startLevelAnalyticsEvent {
        "StartLevel:ControlType:" + ToControlTypeString(mSettingsService.GetControlType())
    };
    
    analytics.AddEvent(startLevelAnalyticsEvent);
}

void UserServices::CompleteLevel(int levelId, int totalNumMovesUsed, int numStars) {
    mProgressService.CompleteLevel(levelId, numStars);
    mLifeService.CompleteLevel();
    
    Pht::ProgressionAnalyticsEvent analyticsEvent {
        Pht::ProgressionStatus::Complete, std::to_string(levelId), totalNumMovesUsed
    };
    
    mEngine.GetAnalytics().AddEvent(analyticsEvent);
}

void UserServices::FailLevel(int levelId, Pht::Optional<int> progressInLevel) {
    mLifeService.FailLevel();
    
    if (progressInLevel.HasValue()) {
        Pht::ProgressionAnalyticsEvent analyticsEvent {
            Pht::ProgressionStatus::Fail, std::to_string(levelId), progressInLevel.GetValue()
        };
        
        mEngine.GetAnalytics().AddEvent(analyticsEvent);
    }
}
