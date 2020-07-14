#ifndef AnalyticsFactory_hpp
#define AnalyticsFactory_hpp

#include <memory>

#include "IAnalytics.hpp"

namespace Pht {
    std::unique_ptr<IAnalytics> CreateAnalyticsApi();
}

#endif
