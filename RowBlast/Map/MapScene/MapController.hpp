#ifndef MapController_hpp
#define MapController_hpp

// Game includes.
#include "MapScene.hpp"
#include "MapViewControllers.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    class UserData;
    class Settings;
    
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
                      Settings& settings);
    
        void Init();
        Command Update();
    
    private:
        Command UpdateMap();
        void UpdateNoLivesDialog();
        void UpdateSettingsMenu();
        Command HandleInput();
        void UpdateTouchingState(const Pht::TouchEvent& touch);
        Command HandleTouch(const Pht::TouchEvent& touch);
        Command HandleLevelClick(int level);
        void Pan(const Pht::TouchEvent& touch);
        void StartPan(const Pht::TouchEvent& touch);
        void UpdateCamera();
        void GoToSettingsMenuState();
        
        enum class State {
            Map,
            NoLivesDialog,
            SettingsMenu
        };
        
        Pht::IEngine& mEngine;
        UserData& mUserData;
        State mState {State::Map};
        MapScene mScene;
        MapViewControllers mMapViewControllers;
        float mCameraXPositionAtPanBegin {0.0f};
        Pht::Vec2 mTouchLocationAtPanBegin {0.0f, 0.0f};
        float mCameraXVelocity {0.0f};
        bool mIsTouching {false};
    };
}

#endif
