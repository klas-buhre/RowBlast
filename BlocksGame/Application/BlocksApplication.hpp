#ifndef BlocksApplication_hpp
#define BlocksApplication_hpp

// Engine includes.
#include "IApplication.hpp"
#include "FadeEffect.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "CommonViewControllers.hpp"
#include "UserData.hpp"
#include "TitleController.hpp"
#include "MapController.hpp"
#include "GameController.hpp"
#include "Settings.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class BlocksApplication: public Pht::IApplication {
    public:
        BlocksApplication(Pht::IEngine& engine);
    
        void OnInitialize() override {}
        void OnUpdate() override;
        void OnRender() override;
        
    private:
        void UpdateScene();
        void UpdateGameScene();
        void HandleTransitions();
        void InsertFadeEffectInActiveScene();
        void BeginFadeToMap();
        void BeginFadeToGame(int level);
        void StartMap();
        void StartGame();
        
        enum class State {
            TitleScene,
            MapScene,
            GameScene
        };
        
        Pht::IEngine& mEngine;
        State mState {State::TitleScene};
        State mNextState {State::TitleScene};
        CommonResources mCommonResources;
        Settings mSettings;
        UserData mUserData;
        CommonViewControllers mCommonViewControllers;
        TitleController mTitleController;
        MapController mMapController;
        GameController mGameController;
        Pht::FadeEffect mFadeEffect;
        int mLevelToStart;
    };
}

#endif
