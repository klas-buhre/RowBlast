#ifndef RowBlastApplication_hpp
#define RowBlastApplication_hpp

// Engine includes.
#include "IApplication.hpp"
#include "FadeEffect.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UserData.hpp"
#include "TitleController.hpp"
#include "MapController.hpp"
#include "GameController.hpp"
#include "Settings.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class RowBlastApplication: public Pht::IApplication {
    public:
        RowBlastApplication(Pht::IEngine& engine);
    
        void OnInitialize() override {}
        void OnUpdate() override;
        
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
        TitleController mTitleController;
        GameController mGameController;
        MapController mMapController;
        Pht::FadeEffect mFadeEffect;
        int mLevelToStart;
    };
}

#endif
