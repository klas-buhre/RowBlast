#include "AnalyticsEvent.hpp"

using namespace Pht;

AnalyticsEvent::AnalyticsEvent(Kind kind) :
    mKind {kind} {}

BusinessEvent::BusinessEvent(const std::string& currency,
                             int amount,
                             const std::string& itemType,
                             const std::string& itemId,
                             const std::string& cartType) :
    AnalyticsEvent {AnalyticsEvent::Kind::Business},
    mCurrency {currency},
    mAmount {amount},
    mItemType {itemType},
    mItemId {itemId},
    mCartType {cartType} {}

ResourceEvent::ResourceEvent(ResourceFlow resourceFlow,
                             const std::string& currency,
                             float amount,
                             const std::string& itemType,
                             const std::string& itemId) :
    AnalyticsEvent {AnalyticsEvent::Kind::Resource},
    mResourceFlow {resourceFlow},
    mCurrency {currency},
    mAmount {amount},
    mItemType {itemType},
    mItemId {itemId} {}

ProgressionEvent::ProgressionEvent(ProgressionStatus progressionStatus,
                                   const std::string& progression,
                                   int score) :
    AnalyticsEvent {AnalyticsEvent::Kind::Progression},
    mProgressionStatus {progressionStatus},
    mProgression {progression},
    mScore {score} {}
