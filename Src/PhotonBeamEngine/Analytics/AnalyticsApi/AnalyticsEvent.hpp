#ifndef AnalyticsEvent_hpp
#define AnalyticsEvent_hpp

#include <string>

namespace Pht {
    class AnalyticsEvent {
    public:
        enum class Kind {
            Business,
            Resource,
            Progression,
            Error
        };
        
        AnalyticsEvent(Kind kind);
        
        Kind GetKind() const {
            return mKind;
        }

    private:
        Kind mKind;
    };

    class BusinessAnalyticsEvent: public AnalyticsEvent {
    public:
        BusinessAnalyticsEvent(const std::string& currency,
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
    
    class ResourceAnalyticsEvent: public AnalyticsEvent {
    public:
        ResourceAnalyticsEvent(ResourceFlow resourceFlow,
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
    
    class ProgressionAnalyticsEvent: public AnalyticsEvent {
    public:
        ProgressionAnalyticsEvent(ProgressionStatus progressionStatus,
                                  const std::string& progression,
                                  int score);
        
        ProgressionStatus mProgressionStatus;
        std::string mProgression;
        int mScore;
    };
    
    enum class ErrorSeverity {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };

    class ErrorAnalyticsEvent: public AnalyticsEvent {
    public:
        ErrorAnalyticsEvent(ErrorSeverity severity, const std::string& message);
        
        ErrorSeverity mSeverity;
        std::string mMessage;
    };
}

#endif
