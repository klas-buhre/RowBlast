#ifndef Purchasing_hpp
#define Purchasing_hpp

#include <memory>

#include "IPurchasing.hpp"

namespace Pht {
    std::unique_ptr<IPurchasing> CreatePurchasingApi();
}

#endif
