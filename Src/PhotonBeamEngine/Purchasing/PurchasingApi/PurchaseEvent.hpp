#ifndef PurchaseEvent_hpp
#define PurchaseEvent_hpp

#include <string>
#include <vector>

namespace Pht {
    struct Product {
        std::string mId;
        std::string mLocalizedPrice;
    };
    
    class PurchaseEvent {
    public:
        enum Kind {
            Complete,
            ProductsResponse,
            Error
        };

        PurchaseEvent(Kind kind);
        virtual ~PurchaseEvent() {}
        
        Kind GetKind() const {
            return mKind;
        }

    private:
        Kind mKind {Kind::Error};
    };

    class PurchaseCompleteEvent: public PurchaseEvent {
    public:
        PurchaseCompleteEvent(const std::string& productId);
        
        const std::string& GetProductId() const {
            return mProductId;
        }
        
    private:
        std::string mProductId;
    };
    
    class ProductsResponseEvent: public PurchaseEvent {
    public:
        ProductsResponseEvent(const std::vector<Product>& products);
        
        const std::vector<Product>& GetProducts() const {
            return mProducts;
        }
        
    private:
        std::vector<Product> mProducts;
    };
    
    enum class PurchaseError {
        Cancelled,
        NotAllowed,
        NoNetworkAccess,
        Other
    };
    
    class PurchaseErrorEvent: public PurchaseEvent {
    public:
        PurchaseErrorEvent(PurchaseError errorCode);
        
        PurchaseError GetErrorCode() const {
            return mErrorCode;
        }
        
    private:
        PurchaseError mErrorCode {PurchaseError::Other};
    };
}

#endif
