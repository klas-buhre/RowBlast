#ifndef StoreMenuController_hpp
#define StoreMenuController_hpp

// Game includes.
#include "StoreMenuView.hpp"
#include "SlidingMenuAnimation.hpp"
#include "CommonResources.hpp"

namespace Pht {
    class IEngine;
    class TouchEvent;
    class FadeEffect;
}

namespace RowBlast {
    class StoreMenuController {
    public:
        class Result {
        public:
            enum Kind {
                None,
                Close,
                PurchaseProduct
            };
            
            Result();
            explicit Result(Kind kind);
            explicit Result(ProductId productId);
            
            bool operator==(const Result& other) const;
            bool operator!=(const Result& other) const;
            ProductId GetProductId() const;
            
            Kind GetKind() const {
                return mKind;
            }

        private:
            Kind mKind {None};
            ProductId mProductId;
        };
        
        StoreMenuController(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            const UserServices& userServices,
                            PotentiallyZoomedScreen potentiallyZoomedScreen);
        
        void SetUp(SlidingMenuAnimation::UpdateFade updateFade,
                   SlidingMenuAnimation::SlideDirection slideDirection);
        void SetFadeEffect(Pht::FadeEffect& fadeEffect);
        Result Update();
        
        Pht::GuiView& GetView() {
            return mView;
        }

    private:
        Result HandleInput();
        Result OnTouch(const Pht::TouchEvent& touchEvent);
        
        Pht::IEngine& mEngine;
        StoreMenuView mView;
        SlidingMenuAnimation mSlidingMenuAnimation;
        Result mDeferredResult;
    };
}

#endif
