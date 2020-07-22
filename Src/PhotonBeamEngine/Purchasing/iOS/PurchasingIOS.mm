#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

#include "IPurchasing.hpp"
#include "PurchasingFactory.hpp"

using namespace Pht;

class PurchasingIOS;

@interface ProductsDelegate: NSObject<SKProductsRequestDelegate> {
    PurchasingIOS* mPurchasing;
};

- (void) setPurchasing:(PurchasingIOS*)purchasing;

@end

@interface PaymentTransactionObserver: NSObject<SKPaymentTransactionObserver> {
    PurchasingIOS* mPurchasing;
};

- (void) setPurchasing:(PurchasingIOS*)purchasing;

@end

class PurchasingIOS: public IPurchasing {
public:
    PurchasingIOS() {
        mPendingTransactions = [NSMutableDictionary dictionary];
        
        mPaymentTransactionObserver = [[PaymentTransactionObserver alloc] init];
        [mPaymentTransactionObserver setPurchasing:this];
        [[SKPaymentQueue defaultQueue] addTransactionObserver:mPaymentTransactionObserver];
    }
    
    void FetchProducts(const std::vector<std::string>& productIds) override {
        NSMutableArray* productIdsArray = [[NSMutableArray alloc] initWithCapacity:productIds.size()];
        
        for (auto& productId: productIds) {
            NSString* productIdString = [NSString stringWithUTF8String:productId.c_str()];
            [productIdsArray addObject:productIdString];
        };
        
        NSSet* productIdsSet = [[NSSet alloc] initWithArray:productIdsArray];
        mProductsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdsSet];
        mProductsDelegate = [[ProductsDelegate alloc] init];
        
        [mProductsDelegate setPurchasing:this];
        mProductsRequest.delegate = mProductsDelegate;
        [mProductsRequest start];
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
    
    void FinishTransaction(const std::string& productId) override {
        NSString* productIdNsString = [NSString stringWithUTF8String:productId.c_str()];
        if ([mPendingTransactions objectForKey:productIdNsString]) {
            [[SKPaymentQueue defaultQueue] finishTransaction:[mPendingTransactions objectForKey:productIdNsString]];
            [mPendingTransactions removeObjectForKey:productIdNsString];
        }
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
    
    void AddPendingTransaction(SKPaymentTransaction* transaction) {
        [mPendingTransactions setObject:transaction forKey:transaction.payment.productIdentifier];
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
    PaymentTransactionObserver* mPaymentTransactionObserver;
    SKProductsRequest* mProductsRequest;
    ProductsDelegate* mProductsDelegate;
    NSArray* mSkProducts;
    NSMutableDictionary* mPendingTransactions;
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
        NSData* priceData = [formattedPriceString
                             dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
        NSString* priceString = [[NSString alloc] initWithData:priceData
                                                  encoding:NSASCIIStringEncoding];
        
        Pht::Product phtProduct {
            .mId = [product.productIdentifier UTF8String],
            .mLocalizedPrice = [priceString UTF8String]
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
    for (SKPaymentTransaction* transaction in transactions) {
        switch (transaction.transactionState) {
            case SKPaymentTransactionStatePurchased: {
                std::string productId = [transaction.payment.productIdentifier UTF8String];
                mPurchasing->PushEvent(std::make_unique<PurchaseCompleteEvent>(productId));
                mPurchasing->AddPendingTransaction(transaction);
                break;
            }
            case SKPaymentTransactionStateFailed:
                if (transaction.error.code == SKErrorPaymentCancelled) {
                    mPurchasing->PushErrorEvent(PurchaseError::Cancelled);
                } else if (transaction.error.code == SKErrorCloudServiceNetworkConnectionFailed) {
                    mPurchasing->PushErrorEvent(PurchaseError::NoNetworkAccess);
                } else {
                    mPurchasing->PushErrorEvent(PurchaseError::Other);
                }
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored: {
                std::string productId = [transaction.originalTransaction.payment.productIdentifier UTF8String];
                mPurchasing->PushEvent(std::make_unique<PurchaseCompleteEvent>(productId));
                mPurchasing->AddPendingTransaction(transaction);
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            }
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
