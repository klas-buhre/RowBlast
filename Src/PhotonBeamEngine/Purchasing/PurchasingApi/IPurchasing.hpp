#ifndef IPurchasing_hpp
#define IPurchasing_hpp

#include <memory>

#include "PurchaseEvent.hpp"

namespace Pht {
    class IPurchasing {
    public:
        virtual ~IPurchasing() {}
    
        virtual void FetchProducts(const std::vector<std::string>& productIds) = 0;
        virtual void StartPurchase(const std::string& productId) = 0;
        virtual bool HasEvents() const = 0;
        virtual std::unique_ptr<PurchaseEvent> PopNextEvent() = 0;
    };
}

#endif
