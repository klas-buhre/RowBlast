#ifndef StoreController_hpp
#define StoreController_hpp

// Engine includes.
#include "FadeEffect.hpp"

// Game includes.
#include "GuiViewManager.hpp"
#include "StoreMenuController.hpp"
#include "PurchaseUnsuccessfulDialogController.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class StoreController {
    public:
        enum class SceneId {
            Map,
            Game
        };
        
        enum class Result {
            None,
            Done
        };
        
        enum class TriggerProduct {
            Coins,
            Moves,
            Lives
        };
        
        StoreController(Pht::IEngine& engine,
                        const CommonResources& commonResources,
                        SceneId sceneId);
        
        void Init(Pht::SceneObject& parentObject);
        void StartPurchaseFlow(TriggerProduct triggerProduct);
        Result Update();

    private:
        enum class ViewController {
            StoreMenu,
            PurchaseUnsuccessfulDialog,
            None
        };

        Result UpdateStoreMenu();
        void UpdatePurchaseUnsuccessfulDialog();
        void SetActiveViewController(ViewController viewController);
        void GoToStoreMenuState(SlidingMenuAnimation::UpdateFade updateFade,
                                SlidingMenuAnimation::SlideDirection slideDirection);
        void GoToPurchaseUnsuccessfulDialogState();
        
        enum class State {
            StoreMenu,
            PurchaseUnsuccessfulDialog,
            Idle
        };

        State mState {State::Idle};
        TriggerProduct mTriggerProduct;
        Pht::FadeEffect mFadeEffect;
        GuiViewManager mViewManager;
        StoreMenuController mStoreMenuController;
        PurchaseUnsuccessfulDialogController mPurchaseUnsuccessfulDialogController;
    };
}

#endif
