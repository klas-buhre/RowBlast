#include "PurchasingService.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "JsonUtil.hpp"
#include "FileStorage.hpp"
#include "IAnalytics.hpp"
#include "AnalyticsEvent.hpp"
#include "IPurchasing.hpp"

using namespace RowBlast;

namespace {
    constexpr auto transactionTimeout = 2.0f;
    constexpr auto maxCoinBalance = 99500;
    const std::string filename {"purchasing.dat"};
    const std::string coinBalanceMember {"coinBalance"};
    
    std::vector<std::pair<ProductId, std::string>> productIdToPhtProductId {
        {ProductId::Currency10Coins, "Currency_10_Coins"},
        {ProductId::Currency50Coins, "Currency_50_Coins"},
        {ProductId::Currency100Coins, "Currency_100_Coins"},
        {ProductId::Currency250Coins, "Currency_250_Coins"},
        {ProductId::Currency500Coins, "Currency_500_Coins"},
    };
    
    std::string ToItemId(ProductId productId) {
        switch (productId) {
            case ProductId::Currency10Coins:
                return "10Coins";
            case ProductId::Currency50Coins:
                return "50Coins";
            case ProductId::Currency100Coins:
                return "100Coins";
            case ProductId::Currency250Coins:
                return "250Coins";
            case ProductId::Currency500Coins:
                return "500Coins";
        }
    }
    
    std::string ToCartType(TriggerProduct triggerProduct) {
        switch (triggerProduct) {
            case TriggerProduct::Coins:
                return "Coins";
            case TriggerProduct::Moves:
                return "Moves";
            case TriggerProduct::Lives:
                return "Lives";
        }
    }
    
    std::string ToErrorMessage(PurchaseFailureReason reason) {
        switch (reason) {
            case PurchaseFailureReason::UserCancelled:
                return "UserCancelledCoinPurchase";
            case PurchaseFailureReason::Other:
                return "CoinPurchaseFailed";
        }
    }
    
    int CalcNumCoinsToWithdraw(CoinWithdrawReason coinWithdrawReason) {
        switch (coinWithdrawReason) {
            case CoinWithdrawReason::AddMoves:
                return PurchasingService::addMovesPriceInCoins;
            case CoinWithdrawReason::RefillLives:
                return PurchasingService::refillLivesPriceInCoins;
        }
    }
    
    std::string ToItemType(CoinWithdrawReason coinWithdrawReason) {
        switch (coinWithdrawReason) {
            case CoinWithdrawReason::AddMoves:
                return "moves";
            case CoinWithdrawReason::RefillLives:
                return "lives";
        }
    }
}

PurchasingService::PurchasingService(Pht::IEngine& engine) :
    mEngine {engine},
    mAllGoldCoinProducts {
        {ProductId::Currency10Coins, 10, "25,00 kr"},
        {ProductId::Currency50Coins, 50, "99,00 kr"},
        {ProductId::Currency100Coins, 100, "179,00 kr"},
        {ProductId::Currency250Coins, 250, "359,00 kr"},
        {ProductId::Currency500Coins, 500, "649,00 kr"}
    } {

    // LoadState();
}

void PurchasingService::FetchProducts(const std::function<void(const std::vector<GoldCoinProduct>&)>& onResponse,
                                      const std::function<void()>& onTimeout) {
    mFetchProductsTransaction.mOnProductsResponse = onResponse;
    mFetchProductsTransaction.mOnTimeout = onTimeout;
    mFetchProductsTransaction.mElapsedTime = 0.0f;
    
    std::vector<std::string> phtProductIds;
    for (auto& entry: productIdToPhtProductId) {
        phtProductIds.push_back(entry.second);
    }
    
    mEngine.GetPurchasing().FetchProducts(phtProductIds);
    mState = State::FetchingProducts;
}

void PurchasingService::Update() {
    switch (mState) {
        case State::FetchingProducts:
            UpdateInFetchingProductsState();
            break;
        case State::PurchasePending:
            UpdateInPurchasePendingState();
            break;
        case State::PurchaseFailure:
            OnPurchaseFailed(PurchaseFailureReason::Other);
            break;
        case State::Idle:
            break;
    }
}

void PurchasingService::UpdateInFetchingProductsState() {
    mFetchProductsTransaction.mElapsedTime += mEngine.GetLastFrameSeconds();
    if (mFetchProductsTransaction.mElapsedTime > transactionTimeout) {
        mFetchProductsTransaction.mOnTimeout();
        mState = State::Idle;
        return;
    }
    
    auto& purchasing = mEngine.GetPurchasing();
    if (purchasing.HasEvents()) {
        auto event = purchasing.PopNextEvent();
        if (event->GetKind() == Pht::PurchaseEvent::ProductsResponse) {
            auto* productsResponseEvent = static_cast<Pht::ProductsResponseEvent*>(event.get());
            auto& phtProducts = productsResponseEvent->GetProducts();
            std::vector<GoldCoinProduct> goldCoinProducts;
            
            for (auto& phtProduct: phtProducts) {
                auto goldCoinProduct = ToGoldCoinProduct(phtProduct);
                if (goldCoinProduct.HasValue()) {
                    goldCoinProducts.push_back(goldCoinProduct.GetValue());
                }
            }

            mFetchProductsTransaction.mOnProductsResponse(goldCoinProducts);
            mState = State::Idle;
        }
    }
}

void PurchasingService::UpdateInPurchasePendingState() {
    mPaymentTransaction.mElapsedTime += mEngine.GetLastFrameSeconds();
    if (mPaymentTransaction.mElapsedTime > transactionTimeout) {
        switch (mPaymentTransaction.mProduct->mId) {
            case ProductId::Currency100Coins:
                OnPurchaseFailed(PurchaseFailureReason::Other);
                break;
            case ProductId::Currency50Coins:
                OnPurchaseFailed(PurchaseFailureReason::UserCancelled);
                break;
            default:
                OnPurchaseSucceeded();
                break;
        }
    }
}

void PurchasingService::OnPurchaseSucceeded() {
    mState = State::Idle;
    mCoinBalance += mPaymentTransaction.mProduct->mNumCoins;
    SaveState();

    Pht::BusinessAnalyticsEvent businessAnalyticsEvent {
        "USD",
        200,
        "GoldCoins",
        ToItemId(mPaymentTransaction.mProduct->mId),
        ToCartType(mPaymentTransaction.mTriggerProduct)
    };
    
    auto& analytics = mEngine.GetAnalytics();
    analytics.AddEvent(businessAnalyticsEvent);
    
    Pht::ResourceAnalyticsEvent resourceAnalyticsEvent {
        Pht::ResourceFlow::Source,
        "coins",
        static_cast<float>(mPaymentTransaction.mProduct->mNumCoins),
        "coins",
        ToItemId(mPaymentTransaction.mProduct->mId)
    };
    
    analytics.AddEvent(resourceAnalyticsEvent);
    
    mPaymentTransaction.mOnPurchaseSucceeded(*mPaymentTransaction.mProduct);
}

void PurchasingService::OnPurchaseFailed(PurchaseFailureReason reason) {
    mState = State::Idle;
    
    Pht::ErrorAnalyticsEvent analyticsEvent {Pht::ErrorSeverity::Info, ToErrorMessage(reason)};
    mEngine.GetAnalytics().AddEvent(analyticsEvent);
    
    mPaymentTransaction.mOnPurchaseFailed(reason);
}

void PurchasingService::StartPurchase(ProductId productId,
                                      TriggerProduct triggerProduct,
                                      const std::function<void(const GoldCoinProduct&)>& onPurchaseSucceeded,
                                      const std::function<void(PurchaseFailureReason)>& onPurchaseFailed) {
    auto* product = GetGoldCoinProduct(productId);
    if (product == nullptr || mState != State::Idle || mCoinBalance >= maxCoinBalance) {
        mState = State::PurchaseFailure;
    } else {
        mState = State::PurchasePending;
    }
    
    mPaymentTransaction.mProduct = product;
    mPaymentTransaction.mTriggerProduct = triggerProduct;
    mPaymentTransaction.mOnPurchaseSucceeded = onPurchaseSucceeded;
    mPaymentTransaction.mOnPurchaseFailed = onPurchaseFailed;
    mPaymentTransaction.mElapsedTime = 0.0f;
}

const GoldCoinProduct* PurchasingService::GetGoldCoinProduct(ProductId productId) const {
    for (auto& product: mAllGoldCoinProducts) {
        if (product.mId == productId) {
            return &product;
        }
    }
    
    return nullptr;
}

Pht::Optional<GoldCoinProduct>
PurchasingService::ToGoldCoinProduct(const Pht::Product& phtProduct) {
    auto mappingEntry = std::find_if(std::begin(productIdToPhtProductId),
                                     std::end(productIdToPhtProductId),
                                     [&phtProduct] (auto& entry) {
                                         return entry.second == phtProduct.mId;
                                     });
    if (mappingEntry == std::end(productIdToPhtProductId)) {
        return Pht::Optional<GoldCoinProduct> {};
    }
    
    auto goldCoinProductId = mappingEntry->first;
    auto existingGoldCoinProduct = std::find_if(std::begin(mAllGoldCoinProducts),
                                                std::end(mAllGoldCoinProducts),
                                                [&goldCoinProductId] (auto& goldCoinProduct) {
                                                    return goldCoinProduct.mId == goldCoinProductId;
                                                });
    if (existingGoldCoinProduct == std::end(mAllGoldCoinProducts)) {
        return Pht::Optional<GoldCoinProduct> {};
    }

    auto goldCoinProduct = *existingGoldCoinProduct;
    goldCoinProduct.mLocalizedPriceString = phtProduct.mLocalizedPrice;
    return goldCoinProduct;
}

void PurchasingService::WithdrawCoins(CoinWithdrawReason coinWithdrawReason) {
    auto numCoinsToWithdraw = CalcNumCoinsToWithdraw(coinWithdrawReason);
    
    mCoinBalance -= numCoinsToWithdraw;
    if (mCoinBalance < 0) {
        mCoinBalance = 0;
    }

    SaveState();
    
    Pht::ResourceAnalyticsEvent resourceAnalyticsEvent {
        Pht::ResourceFlow::Sink,
        "coins",
        static_cast<float>(numCoinsToWithdraw),
        ToItemType(coinWithdrawReason),
        "Consumed"
    };
    mEngine.GetAnalytics().AddEvent(resourceAnalyticsEvent);
}

bool PurchasingService::CanAfford(int priceInCoins) const {
    return mCoinBalance >= priceInCoins;
}

void PurchasingService::SaveState() {
    rapidjson::Document document;
    auto& allocator = document.GetAllocator();
    document.SetObject();
    
    Pht::Json::AddInt(document, coinBalanceMember, mCoinBalance, allocator);

    std::string jsonString;
    Pht::Json::EncodeDocument(document, jsonString);
    Pht::FileStorage::Save(filename, jsonString);
}

bool PurchasingService::LoadState() {
    std::string jsonString;
    if (!Pht::FileStorage::Load(filename, jsonString)) {
        return false;
    }
    
    rapidjson::Document document;
    Pht::Json::ParseDocument(document, jsonString);
    
    mCoinBalance = Pht::Json::ReadInt(document, coinBalanceMember);

    return true;
}
