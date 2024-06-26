#ifndef PurchasingFactory_hpp
#define PurchasingFactory_hpp

#include <memory>

#include "IPurchasing.hpp"

namespace Pht {
    std::unique_ptr<IPurchasing> CreatePurchasingApi();
}

#endif
