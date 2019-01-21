#ifndef Analytics_hpp
#define Analytics_hpp

#include <memory>

#include "IAnalytics.hpp"

namespace Pht {
    void InitAnalytics();
    std::unique_ptr<IAnalytics> CreateAnalyticsApi();
}

#endif
