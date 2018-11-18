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
        void StartEffects();
        void Update();
        
        const MenuButton& GetCloseButton() const {
            return *mCloseButton;
        }
        
        struct Product {
            std::unique_ptr<MenuButton> mButton;
            Pht::SceneObject* mCoinPile {nullptr};
            std::unique_ptr<Pht::SceneObject> mGlowEffect;
            std::unique_ptr<Pht::SceneObject> mTwinklesEffect;
        };
        
        const std::vector<Product>& GetProducts() const {
            return mProducts;
        }
        
    private:
        void CreateProduct(const Pht::Vec3& position,
                           const std::string& numCoins,
                           const std::string& price,
                           Pht::IEngine& engine,
                           const CommonResources& commonResources,
                           PotentiallyZoomedScreen zoom);
        void CreateGlowEffect(Pht::IEngine& engine,
                              Pht::SceneObject& parentObject,
                              Product& product);
        void CreateTwinklesEffect(Pht::IEngine& engine,
                                  Pht::SceneObject& parentObject,
                                  Product& product);

        Pht::IEngine& mEngine;
        std::unique_ptr<MenuButton> mCloseButton;
        std::vector<Product> mProducts;
        float mAnimationTime {0.0f};
    };
}

#endif
