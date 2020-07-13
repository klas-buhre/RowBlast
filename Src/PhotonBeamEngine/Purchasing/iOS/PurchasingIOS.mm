#include "IPurchasing.hpp"
#include "Purchasing.hpp"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

using namespace Pht;

class PurchasingIOS;

@interface ProductsDelegate : NSObject <SKProductsRequestDelegate> {
    PurchasingIOS* mPurchasing;
};

- (void) setPurchasing: (PurchasingIOS*) purchasing;

@end

@interface PaymentTransactionObserver : NSObject <SKPaymentTransactionObserver> {
    PurchasingIOS* mPurchasing;
};

- (void) setPurchasing: (PurchasingIOS*) purchasing;

@end

class PurchasingIOS: public IPurchasing {
public:
    PurchasingIOS() {
        PaymentTransactionObserver *observer = [[PaymentTransactionObserver alloc] init];
        [observer setPurchasing:this];
        [[SKPaymentQueue defaultQueue] addTransactionObserver:observer];
    }
    
    void FetchProducts(const std::vector<std::string>& productIds) override {
        NSMutableArray* productIdsArray = [[NSMutableArray alloc] initWithCapacity:productIds.size()];
        
        for (auto& productId: productIds) {
            NSString* productIdString = [NSString stringWithUTF8String:productId.c_str()];
            [productIdsArray addObject:productIdString];
        };

        NSSet* productIdsSet = [[NSSet alloc] initWithArray:productIdsArray];
        SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdsSet];
        
        // Keep a strong reference to the request to prevent it from being deallocated.
        mProductsRequest = request;
        
        ProductsDelegate* delegate = [[ProductsDelegate alloc] init];
        [delegate setPurchasing:this];

        request.delegate = delegate;
        [request start];
    }
    
    void StartPurchase(const std::string& productId) override {
        if (![SKPaymentQueue canMakePayments]) {
            PushErrorEvent(PurchaseError::NotAllowed);
            return;
        }
        
        SKProduct* product = FindSkProduct(productId);
        if (product == nullptr) {
            PushErrorEvent(PurchaseError::Other);
            return;
        }

        SKPayment* payment = [SKPayment paymentWithProduct:product];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    }

    bool HasEvents() const override {
        return !mEvents.empty();
    }

    std::unique_ptr<PurchaseEvent> PopNextEvent() override {
        auto back = std::move(mEvents.back());
        mEvents.pop_back();
        return back;
    }
    
    void PushEvent(std::unique_ptr<PurchaseEvent> event) {
        mEvents.push_back(std::move(event));
    }
    
    void PushErrorEvent(PurchaseError errorCode) {
        auto event = std::make_unique<PurchaseErrorEvent>(errorCode);
        PushEvent(std::move(event));
    }

    void SetSkProducts(NSArray* products) {
        mSkProducts = products;
    }
    
private:
    SKProduct* FindSkProduct(const std::string& productId) {
        if (mSkProducts == nullptr) {
            return nullptr;
        }
        
        for (NSUInteger i = 0; i < [mSkProducts count]; i++) {
            SKProduct* skProduct = [mSkProducts objectAtIndex:i];
            std::string skProductId = [skProduct.productIdentifier UTF8String];
            if (productId == skProductId) {
                return skProduct;
            }
        }
        
        return nullptr;
    }

    std::vector<std::unique_ptr<PurchaseEvent>> mEvents;
    SKProductsRequest* mProductsRequest;
    NSArray* mSkProducts;
};

@implementation ProductsDelegate

- (void) productsRequest:(SKProductsRequest*)request
         didReceiveResponse:(SKProductsResponse*)response {
    NSArray* skProducts = response.products;
    std::vector<Product> phtProducts;

    for (NSUInteger i = 0; i < [skProducts count]; i++) {
        SKProduct* product = [skProducts objectAtIndex:i];
        
        NSNumberFormatter* numberFormatter = [[NSNumberFormatter alloc] init];
        [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        [numberFormatter setLocale:product.priceLocale];
        NSString* formattedPriceString = [numberFormatter stringFromNumber:product.price];
        
        Pht::Product phtProduct {
            .mId = [product.productIdentifier UTF8String],
            .mLocalizedPrice = [formattedPriceString UTF8String]
        };
        phtProducts.push_back(phtProduct);
    }
    
    auto event = std::make_unique<ProductsResponseEvent>(phtProducts);
    mPurchasing->PushEvent(std::move(event));
    mPurchasing->SetSkProducts(skProducts);
}

- (void) setPurchasing:(PurchasingIOS*)purchasing {
    mPurchasing = purchasing;
}

@end

@implementation PaymentTransactionObserver

- (void) paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions {
    for (SKPaymentTransaction *transaction in transactions) {
        switch (transaction.transactionState) {
            case SKPaymentTransactionStatePurchased:
                mPurchasing->PushEvent(std::make_unique<PurchaseEvent>(PurchaseEvent::Complete));
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                if ([[transaction error] code] == SKErrorPaymentCancelled) {
                    mPurchasing->PushErrorEvent(PurchaseError::Cancelled);
                } else {
                    mPurchasing->PushErrorEvent(PurchaseError::Other);
                }
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            default:
                break;
        }
    }
}

- (void) setPurchasing:(PurchasingIOS*)purchasing {
    mPurchasing = purchasing;
}

@end

std::unique_ptr<IPurchasing> Pht::CreatePurchasingApi() {
    return std::make_unique<PurchasingIOS>();
}
