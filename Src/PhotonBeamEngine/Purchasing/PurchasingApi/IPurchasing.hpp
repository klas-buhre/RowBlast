#ifndef IPurchasing_hpp
#define IPurchasing_hpp

#include <string>
#include <memory>
#include <vector>

namespace Pht {
    struct Product {
        std::string mId;
        std::string mLocalizedPrice;
    };
    
    struct PurchaseEvent {
        virtual ~PurchaseEvent() {}
        
        enum Kind {
            Complete,
            Error,
            Products
        };
        
        Kind mKind {Kind::Error};
    };
    
    struct ProductsEvent: public PurchaseEvent {
        std::vector<Product> mProducts;
    };
    
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
