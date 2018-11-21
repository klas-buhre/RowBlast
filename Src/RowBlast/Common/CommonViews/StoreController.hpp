#ifndef StoreController_hpp
#define StoreController_hpp

// Engine includes.
#include "FadeEffect.hpp"

// Game includes.
#include "GuiViewManager.hpp"
#include "StoreMenuController.hpp"

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
            None
        };

        Result UpdateStoreMenu();
        void SetActiveViewController(ViewController viewController);
        
        enum class State {
            StoreMenu,
            Idle
        };

        State mState {State::Idle};
        TriggerProduct mTriggerProduct;
        Pht::FadeEffect mFadeEffect;
        GuiViewManager mViewManager;
        StoreMenuController mStoreMenuController;
    };
}

#endif
