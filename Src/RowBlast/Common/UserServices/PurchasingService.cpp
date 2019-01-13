#include "PurchasingService.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "JsonUtil.hpp"
#include "FileStorage.hpp"

using namespace RowBlast;

namespace {
    constexpr auto transactionTimeout {2.0f};
    constexpr auto maxCoinBalance {99500};
    const std::string filename {"purchasing.dat"};
    const std::string coinBalanceMember {"coinBalance"};
}

PurchasingService::PurchasingService(Pht::IEngine& engine) :
    mEngine {engine},
    mProducts {
        {ProductId::Currency10Coins, 10, "20,00 kr"},
        {ProductId::Currency50Coins, 50, "85,00 kr"},
        {ProductId::Currency100Coins, 100, "159,00 kr"},
        {ProductId::Currency250Coins, 250, "319,00 kr"},
        {ProductId::Currency500Coins, 500, "595,00 kr"}
    } {

    // LoadState();
}

void PurchasingService::Update() {
    switch (mState) {
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

void PurchasingService::UpdateInPurchasePendingState() {
    mTransaction.mElapsedTime += mEngine.GetLastFrameSeconds();
    
    if (mTransaction.mElapsedTime > transactionTimeout) {
        switch (mTransaction.mProduct->mId) {
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
    
    mCoinBalance += mTransaction.mProduct->mNumCoins;
    SaveState();
    
    mTransaction.mOnPurchaseSucceeded(*mTransaction.mProduct);
}

void PurchasingService::OnPurchaseFailed(PurchaseFailureReason reason) {
    mState = State::Idle;
    mTransaction.mOnPurchaseFailed(reason);
}

void PurchasingService::StartPurchase(ProductId productId,
                                      const std::function<void(const GoldCoinProduct&)>& onPurchaseSucceeded,
                                      const std::function<void(PurchaseFailureReason)>& onPurchaseFailed) {
    auto* product {GetGoldCoinProduct(productId)};
    
    if (product == nullptr || mState != State::Idle || mCoinBalance >= maxCoinBalance) {
        mState = State::PurchaseFailure;
    } else {
        mState = State::PurchasePending;
    }
    
    mTransaction.mProduct = product;
    mTransaction.mOnPurchaseSucceeded = onPurchaseSucceeded;
    mTransaction.mOnPurchaseFailed = onPurchaseFailed;
    mTransaction.mElapsedTime = 0.0f;
}

const GoldCoinProduct* PurchasingService::GetGoldCoinProduct(ProductId productId) const {
    for (auto& product: mProducts) {
        if (product.mId == productId) {
            return &product;
        }
    }
    
    return nullptr;
}

void PurchasingService::WithdrawCoins(int numCoinsToWithdraw) {
    mCoinBalance -= numCoinsToWithdraw;
    
    if (mCoinBalance < 0) {
        mCoinBalance = 0;
    }

    SaveState();
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
