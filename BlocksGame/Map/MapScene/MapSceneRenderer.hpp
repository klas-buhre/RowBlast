#ifndef MapSceneRenderer_hpp
#define MapSceneRenderer_hpp

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class MapHud;
    class MapViewControllers;
    class UserData;
    
    class MapSceneRenderer {
    public:
        MapSceneRenderer(Pht::IEngine& engine,
                         const MapHud& hud,
                         const MapViewControllers& mapViewControllers,
                         const UserData& userData);
    
        void RenderFrame();
        
    private:
        void RenderHud();
        void RenderViews();
        
        Pht::IEngine& mEngine;
        const MapHud& mHud;
        const MapViewControllers& mMapViewControllers;
        const UserData& mUserData;
    };
}

#endif
