#include "UserServices.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IAnalytics.hpp"
#include "AnalyticsEvent.hpp"

using namespace RowBlast;

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
    
    Pht::ProgressionAnalyticsEvent analyticsEvent {
        Pht::ProgressionStatus::Start, std::to_string(levelId)
    };
    
    mEngine.GetAnalytics().AddEvent(analyticsEvent);
}

void UserServices::CompleteLevel(int levelId, int totalNumMovesUsed, int numStars) {
    mProgressService.CompleteLevel(levelId, numStars);
    mLifeService.CompleteLevel();
    
    Pht::ProgressionAnalyticsEvent analyticsEvent {
        Pht::ProgressionStatus::Complete, std::to_string(levelId), totalNumMovesUsed
    };
    
    mEngine.GetAnalytics().AddEvent(analyticsEvent);
}

void UserServices::FailLevel(int levelId, Pht::Optional<int> progress) {
    mLifeService.FailLevel();
    
    if (progress.HasValue()) {
        Pht::ProgressionAnalyticsEvent analyticsEvent {
            Pht::ProgressionStatus::Fail, std::to_string(levelId), progress.GetValue()
        };
        
        mEngine.GetAnalytics().AddEvent(analyticsEvent);
    }
}
