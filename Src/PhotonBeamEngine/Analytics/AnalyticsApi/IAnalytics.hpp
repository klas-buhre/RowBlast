#ifndef IAnalytics_hpp
#define IAnalytics_hpp

#define ANALYTICS_ENABLED

namespace Pht {
    class AnalyticsEvent;
    
    class IAnalytics {
    public:
        virtual ~IAnalytics() {}
        
        virtual void EnableEventSubmission() = 0;
        virtual void AddEvent(const AnalyticsEvent& event) = 0;
    };
}

#endif
