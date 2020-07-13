#ifndef StoreMenuView_hpp
#define StoreMenuView_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"
#include "PurchasingService.hpp"

namespace Pht {
    class IEngine;
    class TextComponent;
}

namespace RowBlast {
    class UserServices;
    class IGuiLightProvider;
    
    class StoreMenuView: public Pht::GuiView {
    public:
        StoreMenuView(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      const UserServices& userServices,
                      IGuiLightProvider& guiLightProvider,
                      PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void SetUp();
        void Update();
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        struct ProductSection {
            ProductId mProductId;
            std::unique_ptr<MenuButton> mPurchaseButton;
            Pht::SceneObject* mContainer {nullptr};
            Pht::TextComponent* mLocalizedPriceText {nullptr};
            Pht::SceneObject* mCoinPilesAndStacks {nullptr};
            std::unique_ptr<Pht::SceneObject> mGlowEffect;
            std::unique_ptr<Pht::SceneObject> mTwinklesEffect;
        };
        
        std::vector<ProductSection>& GetProductSections() {
            return mProductSections;
        }
        
    private:
        void CreateRenderables(Pht::IEngine& engine,
                               const CommonResources& commonResources);
        void CreateProductSection(const Pht::Vec3& position,
                                  float scale,
                                  ProductId productId,
                                  const Pht::Vec3& twinklesVolume,
                                  const Pht::Vec2& glowSize,
                                  Pht::IEngine& engine,
                                  const CommonResources& commonResources,
                                  PotentiallyZoomedScreen zoom);
        void CreateCoinPilesAndStacks(Pht::SceneObject& parentObject,
                                      ProductSection& productSection,
                                      float scale);
        void CreateCoinsFor10CoinsProduct(Pht::SceneObject& parentObject);
        void CreateCoinsFor50CoinsProduct(Pht::SceneObject& parentObject);
        void CreateCoinsFor100CoinsProduct(Pht::SceneObject& parentObject);
        void CreateCoinsFor250CoinsProduct(Pht::SceneObject& parentObject);
        void CreateCoinsFor500CoinsProduct(Pht::SceneObject& parentObject);
        void CreateCoin(const Pht::Vec3& position, Pht::SceneObject& parentObject);
        void CreateShortCoinStack(const Pht::Vec3& position, Pht::SceneObject& parentObject);
        void CreateTallCoinStack(const Pht::Vec3& position, Pht::SceneObject& parentObject);
        void CreateTallerCoinStack(const Pht::Vec3& position, Pht::SceneObject& parentObject);
        void CreateGlowEffect(Pht::IEngine& engine,
                              Pht::SceneObject& parentObject,
                              ProductSection& productSection,
                              const Pht::Vec2& size);
        void CreateTwinklesEffect(Pht::IEngine& engine,
                                  Pht::SceneObject& parentObject,
                                  ProductSection& productSection,
                                  const Pht::Vec3& twinklesVolume);
        void UpdateAnimations();
        void UpdateCoinBalanceText();

        Pht::IEngine& mEngine;
        const UserServices& mUserServices;
        IGuiLightProvider& mGuiLightProvider;
        std::unique_ptr<Pht::RenderableObject> mCoinRenderable;
        std::unique_ptr<Pht::RenderableObject> mLightCoinRenderable;
        std::unique_ptr<Pht::RenderableObject> mShortCoinStackRenderable;
        std::unique_ptr<Pht::RenderableObject> mTallCoinStackRenderable;
        std::unique_ptr<Pht::RenderableObject> mTallerCoinStackRenderable;
        std::unique_ptr<Pht::RenderableObject> mCoinPileRenderable;
        std::unique_ptr<MenuButton> mCloseButton;
        std::vector<ProductSection> mProductSections;
        float mAnimationTime {0.0f};
        int mCoinBalance {0};
        Pht::TextComponent* mCoinBalanceText {nullptr};
    };
}

#endif
