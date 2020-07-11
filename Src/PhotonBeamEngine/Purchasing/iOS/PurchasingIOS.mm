#include "IPurchasing.hpp"

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

using namespace Pht;

class Purchasing;

@interface ProductsDelegate : NSObject <SKProductsRequestDelegate> {
    Purchasing* mPurchasing;
};

- (void) setPurchasing: (Purchasing*) purchasing;

@end

class Purchasing: public IPurchasing {
public:
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
    
private:
    std::vector<std::unique_ptr<PurchaseEvent>> mEvents;
    SKProductsRequest* mProductsRequest;
};

@implementation ProductsDelegate

- (void) productsRequest:(SKProductsRequest*)request
         didReceiveResponse:(SKProductsResponse*)response {
    NSArray* products = response.products;
    auto event = std::make_unique<ProductsEvent>();

    for (NSUInteger i = 0; i < [products count]; i++) {
        SKProduct* product = [products objectAtIndex:i];
        
        NSNumberFormatter* numberFormatter = [[NSNumberFormatter alloc] init];
        [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        [numberFormatter setLocale:product.priceLocale];
        NSString* formattedPriceString = [numberFormatter stringFromNumber:product.price];
        
        Pht::Product phtProduct {
            .mId = [product.productIdentifier UTF8String],
            .mLocalizedPrice = [formattedPriceString UTF8String]
        };
        event->mProducts.push_back(phtProduct);
    }
    
    mPurchasing->PushEvent(std::move(event));
}

- (void) setPurchasing:(Purchasing*)purchasing {
    mPurchasing = purchasing;
}

@end
