#ifndef PurchasingService_hpp
#define PurchasingService_hpp

#include <string>
#include <vector>
#include <functional>

namespace Pht {
    class IEngine;
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
        
        static constexpr auto addMovesPriceInCoins {10};
        static constexpr auto refillLivesPriceInCoins {12};
        
    private:
        void UpdateInPurchasePendingState();
        void OnPurchaseSucceeded();
        void OnPurchaseFailed(PurchaseFailureReason reason);
        void SaveState();
        bool LoadState();
        
        enum class State {
            PurchasePending,
            PurchaseFailure,
            Idle
        };
        
        struct Transaction {
            const GoldCoinProduct* mProduct {nullptr};
            TriggerProduct mTriggerProduct {TriggerProduct::Coins};
            std::function<void(const GoldCoinProduct&)> mOnPurchaseSucceeded;
            std::function<void(PurchaseFailureReason)> mOnPurchaseFailed;
            float mElapsedTime {0.0f};
        };

        Pht::IEngine& mEngine;
        State mState {State::Idle};
        int mCoinBalance {0};
        Transaction mTransaction;
        std::vector<GoldCoinProduct> mProducts;
    };
}

#endif
