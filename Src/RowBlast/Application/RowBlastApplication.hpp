#ifndef RowBlastApplication_hpp
#define RowBlastApplication_hpp

// Engine includes.
#include "IApplication.hpp"
#include "FadeEffect.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UserServices.hpp"
#include "TitleController.hpp"
#include "MapController.hpp"
#include "GameController.hpp"
#include "AcceptTermsController.hpp"
#include "Universe.hpp"

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
        enum class MapInitialState {
            Map,
            UfoAnimation,
            LevelGoalDialog
        };

        void SetUpRenderer();
        void SetUpAudio();
        void UpdateScene();
        void UpdateAcceptTermsScene();
        void UpdateTitleScene();
        void UpdateMapScene();
        void UpdateGameScene();
        void HandleTransitions();
        void InsertFadeEffectInActiveScene();
        void BeginFadingToMap(MapInitialState mapInitialState);
        void BeginFadingToGame(int level);
        void StartAcceptTermsScene();
        void StartTitleScene();
        void StartMapScene();
        void StartGameScene();
        
        enum class State {
            AcceptTermsScene,
            TitleScene,
            MapScene,
            GameScene
        };
        
        Pht::IEngine& mEngine;
        State mState {State::TitleScene};
        State mNextState {State::TitleScene};
        CommonResources mCommonResources;
        UserServices mUserServices;
        Universe mUniverse;
        AcceptTermsController mAcceptTermsController;
        TitleController mTitleController;
        GameController mGameController;
        MapController mMapController;
        Pht::FadeEffect mFadeEffect;
        int mLevelToStart;
        MapInitialState mMapInitialState {MapInitialState::Map};
    };
}

#endif
