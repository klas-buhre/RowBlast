#ifndef MapSceneRenderer_hpp
#define MapSceneRenderer_hpp

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class MapViewControllers;
    
    class MapSceneRenderer {
    public:
        MapSceneRenderer(Pht::IEngine& engine, const MapViewControllers& mapViewControllers);
    
        void RenderFrame();
        
    private:
        void RenderViews();
        
        Pht::IEngine& mEngine;
        const MapViewControllers& mMapViewControllers;
    };
}

#endif
