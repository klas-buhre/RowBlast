#ifndef PurchasingService_hpp
#define PurchasingService_hpp

#include <string>
#include <vector>
#include <functional>

// Engine includes.
#include "Optional.hpp"

namespace Pht {
    class IEngine;
    class Product;
}

namespace RowBlast {
    enum class ProductId {
        Currency10Coins,
        Currency50Coins,
        Currency100Coins,
        Currency250Coins,
        Currency500Coins
    };

    struct GoldCoinProduct {
        ProductId mId {ProductId::Currency10Coins};
        int mNumCoins {0};
        std::string mLocalizedPriceString;
    };
    
    enum class PurchaseFailureReason {
        UserCancelled,
        Other
    };
    
    enum class TriggerProduct {
        Coins,
        Moves,
        Lives
    };
    
    enum class CoinWithdrawReason {
        AddMoves,
        RefillLives
    };
    
    class PurchasingService {
    public:
        PurchasingService(Pht::IEngine& engine);
        
        void Update();
        void FetchProducts(const std::function<void(const std::vector<GoldCoinProduct>&)>& onResponse,
                           const std::function<void()>& onTimeout);
        void StartPurchase(ProductId productId,
                           TriggerProduct triggerProduct,
                           const std::function<void(const GoldCoinProduct&)>& onPurchaseSucceeded,
                           const std::function<void(PurchaseFailureReason)>& onPurchaseFailed);
        const GoldCoinProduct* GetGoldCoinProduct(ProductId productId) const;
        void WithdrawCoins(CoinWithdrawReason coinWithdrawReason);
        bool CanAfford(int priceInCoins) const;
        
        int GetCoinBalance() const {
            return mCoinBalance;
        }
        
        static constexpr auto addMovesPriceInCoins = 10;
        static constexpr auto refillLivesPriceInCoins = 12;
        
    private:
        void UpdateInFetchingProductsState();
        void UpdateInPurchasePendingState();
        void OnPurchaseSucceeded();
        void OnPurchaseFailed(PurchaseFailureReason reason);
        Pht::Optional<GoldCoinProduct> ToGoldCoinProduct(const Pht::Product& phtProduct);
        void SaveState();
        bool LoadState();
        
        enum class State {
            FetchingProducts,
            PurchasePending,
            PurchaseFailure,
            Idle
        };
        
        struct PaymentTransaction {
            const GoldCoinProduct* mProduct {nullptr};
            TriggerProduct mTriggerProduct {TriggerProduct::Coins};
            std::function<void(const GoldCoinProduct&)> mOnPurchaseSucceeded;
            std::function<void(PurchaseFailureReason)> mOnPurchaseFailed;
            float mElapsedTime {0.0f};
        };
        
        struct FetchProductsTransaction {
            std::function<void(const std::vector<GoldCoinProduct>&)> mOnProductsResponse;
            std::function<void()> mOnTimeout;
            float mElapsedTime {0.0f};
        };

        Pht::IEngine& mEngine;
        State mState {State::Idle};
        int mCoinBalance {0};
        PaymentTransaction mPaymentTransaction;
        FetchProductsTransaction mFetchProductsTransaction;
        std::vector<GoldCoinProduct> mAllGoldCoinProducts;
    };
}

#endif
