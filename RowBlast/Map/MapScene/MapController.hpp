#ifndef MapController_hpp
#define MapController_hpp

// Game includes.
#include "MapScene.hpp"
#include "MapViewControllers.hpp"
#include "Avatar.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class UserData;
    class Settings;
    class LevelResources;
    class PieceResources;
    
    class MapController {
    public:
        class Command {
        public:
            enum Kind {
                StartGame,
                None
            };
            
            Command(Kind kind, int level = 0);
            
            int GetLevel() const;
            
            Kind GetKind() const {
                return mKind;
            }

        private:
            Kind mKind {None};
            int mLevel;
        };
    
        MapController(Pht::IEngine& engine,
                      const CommonResources& commonResources,
                      UserData& userData,
                      Settings& settings,
                      const LevelResources& levelResources,
                      PieceResources& pieceResources);
    
        void Init();
        Command Update();
    
    private:
        void UpdateMap();
        Command UpdateLevelStartDialog();
        void UpdateNoLivesDialog();
        void UpdateSettingsMenu();
        void HandleInput();
        void UpdateTouchingState(const Pht::TouchEvent& touch);
        void HandleTouch(const Pht::TouchEvent& touch);
        void HandleLevelClick(int levelIndex);
        void Pan(const Pht::TouchEvent& touch);
        void StartPan(const Pht::TouchEvent& touch);
        void UpdateCamera();
        void GoToSettingsMenuState();
        
        enum class State {
            Map,
            LevelStartDialog,
            NoLivesDialog,
            SettingsMenu
        };
        
        Pht::IEngine& mEngine;
        UserData& mUserData;
        const LevelResources& mLevelResources;
        State mState {State::Map};
        MapScene mScene;
        Avatar mAvatar;
        MapViewControllers mMapViewControllers;
        float mCameraXPositionAtPanBegin {0.0f};
        Pht::Vec2 mTouchLocationAtPanBegin {0.0f, 0.0f};
        float mCameraXVelocity {0.0f};
        bool mIsTouching {false};
        int mLevelToStart;
    };
}

#endif
