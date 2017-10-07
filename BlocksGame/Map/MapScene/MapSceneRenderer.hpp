#ifndef MapSceneRenderer_hpp
#define MapSceneRenderer_hpp

namespace Pht {
    class IRenderer;
}

namespace BlocksGame {
    class MapScene;
    class MapPin;
    class MapHud;
    class MapViewControllers;
    class UserData;
    
    class MapSceneRenderer {
    public:
        MapSceneRenderer(Pht::IRenderer& engineRenderer,
                         const MapScene& scene,
                         const MapHud& hud,
                         const MapViewControllers& mapViewControllers,
                         const UserData& userData);
    
        void RenderFrame();
        
    private:
        void RenderPins();
        void RenderPin(MapPin& pin);
        void RenderHud();
        void RenderViews();
        
        Pht::IRenderer& mEngineRenderer;
        const MapScene& mScene;
        const MapHud& mHud;
        const MapViewControllers& mMapViewControllers;
        const UserData& mUserData;
    };
}

#endif
