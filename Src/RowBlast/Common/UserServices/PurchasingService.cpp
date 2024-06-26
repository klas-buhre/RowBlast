#include "PurchasingService.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "JsonUtil.hpp"
#include "FileStorage.hpp"
#include "IAnalytics.hpp"
#include "AnalyticsEvent.hpp"
#include "IPurchasing.hpp"
#include "App.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fetchProductsTimeout = 20.0f;
    constexpr auto coinBalanceAtFirstLaunch = 50;
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
    
    std::string ToCartType(StoreTrigger storeTrigger) {
        switch (storeTrigger) {
            case StoreTrigger::Coins:
                return "Coins";
            case StoreTrigger::Moves:
                return "Moves";
            case StoreTrigger::Lives:
                return "Lives";
        }
    }
    
    std::string ToErrorMessage(Pht::PurchaseError error) {
        switch (error) {
            case Pht::PurchaseError::Cancelled:
                return "UserCancelledPurchase";
            case Pht::PurchaseError::NotAllowed:
                return "PurchasesDisabledOnDevice";
            case Pht::PurchaseError::NoNetworkAccess:
                return "NoNetworkAccess";
            case Pht::PurchaseError::Other:
                return "PurchaseFailed";
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
        {ProductId::Currency10Coins, 10, ""},
        {ProductId::Currency50Coins, 50, ""},
        {ProductId::Currency100Coins, 100, ""},
        {ProductId::Currency250Coins, 250, ""},
        {ProductId::Currency500Coins, 500, ""}
    } {
    
    if (Pht::App::IsFirstLaunch()) {
        mCoinBalance = coinBalanceAtFirstLaunch;
    } else {
        LoadState();
    }
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
    if (mFirstUpdate) {
        HandleUnfinishedTransactions();
    }

    switch (mState) {
        case State::FetchingProducts:
            UpdateInFetchingProductsState();
            break;
        case State::PurchasePending:
            UpdateInPurchasePendingState();
            break;
        case State::PurchaseFailure:
            OnPurchaseFailed();
            break;
        case State::Idle:
            break;
    }
}

void PurchasingService::UpdateInFetchingProductsState() {
    mFetchProductsTransaction.mElapsedTime += mEngine.GetLastFrameSeconds();
    if (mFetchProductsTransaction.mElapsedTime > fetchProductsTimeout) {
        mFetchProductsTransaction.mOnTimeout();
        mState = State::Idle;
        return;
    }
    
    auto& purchasing = mEngine.GetPurchasing();
    while (purchasing.HasEvents()) {
        auto event = purchasing.PopNextEvent();
        if (event->GetKind() == Pht::PurchaseEvent::ProductsResponse) {
            auto* productsResponseEvent = static_cast<const Pht::ProductsResponseEvent*>(event.get());
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
    auto& purchasing = mEngine.GetPurchasing();
    while (purchasing.HasEvents()) {
        auto event = purchasing.PopNextEvent();
        switch (event->GetKind()) {
            case Pht::PurchaseEvent::Complete: {
                auto* purchaseCompleteEvent = static_cast<const Pht::PurchaseCompleteEvent*>(event.get());
                auto product = ToGoldCoinProduct(purchaseCompleteEvent->GetProductId());
                if (product.HasValue()) {
                    OnPurchaseSucceeded(product.GetValue());
                    mPaymentTransaction.mOnPurchaseSucceeded(product.GetValue());
                    purchasing.FinishTransaction(purchaseCompleteEvent->GetProductId());
                }
                break;
            }
            case Pht::PurchaseEvent::Error: {
                auto* errorEvent = static_cast<const Pht::PurchaseErrorEvent*>(event.get());
                mPaymentTransaction.mError = errorEvent->GetErrorCode();
                OnPurchaseFailed();
                break;
            }
            default:
                break;
        }
    }
}

void PurchasingService::OnPurchaseSucceeded(const GoldCoinProduct& product) {
    mState = State::Idle;
    mCoinBalance += product.mNumCoins;
    SaveState();

    Pht::BusinessAnalyticsEvent businessAnalyticsEvent {
        "USD", // TODO: fix
        2,     // TODO: fix
        "GoldCoins",
        ToItemId(product.mId),
        ToCartType(mPaymentTransaction.mStoreTrigger)
    };
    
    auto& analytics = mEngine.GetAnalytics();
    analytics.AddEvent(businessAnalyticsEvent);
    
    Pht::ResourceAnalyticsEvent resourceAnalyticsEvent {
        Pht::ResourceFlow::Source,
        "coins",
        static_cast<float>(product.mNumCoins),
        "coins",
        ToItemId(product.mId)
    };
    
    analytics.AddEvent(resourceAnalyticsEvent);
}

void PurchasingService::OnPurchaseFailed() {
    mState = State::Idle;
    
    Pht::ErrorAnalyticsEvent analyticsEvent {
        Pht::ErrorSeverity::Info,
        ToErrorMessage(mPaymentTransaction.mError)
    };
    mEngine.GetAnalytics().AddEvent(analyticsEvent);
    
    mPaymentTransaction.mOnPurchaseFailed(mPaymentTransaction.mError);
}

void PurchasingService::StartPurchase(ProductId productId,
                                      StoreTrigger storeTrigger,
                                      const std::function<void(const GoldCoinProduct&)>& onPurchaseSucceeded,
                                      const std::function<void(Pht::PurchaseError)>& onPurchaseFailed) {
    mPaymentTransaction.mStoreTrigger = storeTrigger;
    mPaymentTransaction.mOnPurchaseSucceeded = onPurchaseSucceeded;
    mPaymentTransaction.mOnPurchaseFailed = onPurchaseFailed;
    mPaymentTransaction.mError = Pht::PurchaseError::Other;
    
    if (GetGoldCoinProduct(productId) == nullptr || mState != State::Idle ||
        mCoinBalance >= maxCoinBalance) {
        
        mState = State::PurchaseFailure;
        return;
    }
    
    auto mappingEntry = std::find_if(std::begin(productIdToPhtProductId),
                                     std::end(productIdToPhtProductId),
                                     [productId] (auto& entry) {
                                         return productId == entry.first;
                                     });
    if (mappingEntry == std::end(productIdToPhtProductId)) {
        mState = State::PurchaseFailure;
        return;
    }
    
    auto phtProductId = mappingEntry->second;
    mEngine.GetPurchasing().StartPurchase(phtProductId);
    mState = State::PurchasePending;
}

void PurchasingService::HandleUnfinishedTransactions() {
    auto& purchasing = mEngine.GetPurchasing();
    while (purchasing.HasEvents()) {
        auto event = purchasing.PopNextEvent();
        if (event->GetKind() == Pht::PurchaseEvent::Complete) {
            auto* purchaseCompleteEvent = static_cast<const Pht::PurchaseCompleteEvent*>(event.get());
            auto product = ToGoldCoinProduct(purchaseCompleteEvent->GetProductId());
            if (product.HasValue()) {
                OnPurchaseSucceeded(product.GetValue());
                purchasing.FinishTransaction(purchaseCompleteEvent->GetProductId());
            }
        }
    }
    
    mFirstUpdate = false;
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
PurchasingService::ToGoldCoinProduct(const std::string& phtProductId) {
    auto mappingEntry = std::find_if(std::begin(productIdToPhtProductId),
                                     std::end(productIdToPhtProductId),
                                     [&phtProductId] (auto& entry) {
                                         return entry.second == phtProductId;
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

    return *existingGoldCoinProduct;
}

Pht::Optional<GoldCoinProduct>
PurchasingService::ToGoldCoinProduct(const Pht::Product& phtProduct) {
    auto goldCoinProduct = ToGoldCoinProduct(phtProduct.mId);
    if (!goldCoinProduct.HasValue()) {
        return Pht::Optional<GoldCoinProduct> {};
    }
        
    goldCoinProduct.GetValue().mLocalizedPriceString = phtProduct.mLocalizedPrice;
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
