#ifndef StoreMenuView_hpp
#define StoreMenuView_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "GuiView.hpp"

// Game includes.
#include "MenuButton.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class StoreMenuView: public Pht::GuiView {
    public:
        StoreMenuView(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void Init();
        void Update();
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        struct Product {
            std::unique_ptr<MenuButton> mButton;
            Pht::SceneObject* mCoinPilesAndStacks {nullptr};
            std::unique_ptr<Pht::SceneObject> mGlowEffect;
            std::unique_ptr<Pht::SceneObject> mTwinklesEffect;
        };
        
        const std::vector<Product>& GetProducts() const {
            return mProducts;
        }
        
    private:
        void CreateRenderables(Pht::IEngine& engine,
                               const CommonResources& commonResources);
        void CreateProduct(const Pht::Vec3& position,
                           float scale,
                           int numCoins,
                           const std::string& price,
                           const Pht::Vec3& twinklesVolume,
                           const Pht::Vec2& glowSize,
                           Pht::IEngine& engine,
                           const CommonResources& commonResources,
                           PotentiallyZoomedScreen zoom);
        void CreateCoinPilesAndStacks(Pht::SceneObject& parentObject,
                                      Product& product,
                                      int numCoins,
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
                              Product& product,
                              const Pht::Vec2& size);
        void CreateTwinklesEffect(Pht::IEngine& engine,
                                  Pht::SceneObject& parentObject,
                                  Product& product,
                                  const Pht::Vec3& twinklesVolume);

        Pht::IEngine& mEngine;
        std::unique_ptr<Pht::RenderableObject> mCoinRenderable;
        std::unique_ptr<Pht::RenderableObject> mLightCoinRenderable;
        std::unique_ptr<Pht::RenderableObject> mShortCoinStackRenderable;
        std::unique_ptr<Pht::RenderableObject> mTallCoinStackRenderable;
        std::unique_ptr<Pht::RenderableObject> mTallerCoinStackRenderable;
        std::unique_ptr<Pht::RenderableObject> mCoinPileRenderable;
        std::unique_ptr<MenuButton> mCloseButton;
        std::vector<Product> mProducts;
        float mAnimationTime {0.0f};
    };
}

#endif
