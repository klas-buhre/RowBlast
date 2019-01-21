#ifndef AnalyticsEvent_hpp
#define AnalyticsEvent_hpp

#include <string>

namespace Pht {
    class AnalyticsEvent {
    public:
        enum class Kind {
            Business,
            Resource,
            Progression
        };
        
        AnalyticsEvent(Kind kind);
        
        Kind GetKind() const {
            return mKind;
        }

    private:
        Kind mKind;
    };

    class BusinessEvent: public AnalyticsEvent {
    public:
        BusinessEvent(const std::string& currency,
                      int amount, // In cents.
                      const std::string& itemType,
                      const std::string& itemId,
                      const std::string& cartType);
        
        std::string mCurrency;
        int mAmount;
        std::string mItemType;
        std::string mItemId;
        std::string mCartType;
    };

    enum class ResourceFlow {
        Source,
        Sink
    };
    
    class ResourceEvent: public AnalyticsEvent {
    public:
        ResourceEvent(ResourceFlow resourceFlow,
                      const std::string& currency,
                      float amount,
                      const std::string& itemType,
                      const std::string& itemId);
        
        ResourceFlow mResourceFlow;
        std::string mCurrency;
        float mAmount;
        std::string mItemType;
        std::string mItemId;
    };
    
    enum class ProgressionStatus {
        Start,
        Fail,
        Complete
    };
    
    class ProgressionEvent: public AnalyticsEvent {
    public:
        ProgressionEvent(ProgressionStatus progressionStatus,
                         const std::string& progression,
                         int score);
        
        ProgressionStatus mProgressionStatus;
        std::string mProgression;
        int mScore;
    };
}

#endif
