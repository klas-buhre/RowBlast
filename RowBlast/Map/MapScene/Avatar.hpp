#ifndef Avatar_hpp
#define Avatar_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"
#include "SceneResources.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class MapScene;
    class CommonResources;
    
    class Avatar {
    public:
        Avatar(Pht::IEngine& engine, MapScene& mapScene, const CommonResources& commonResources);
        
        void Init();
        void SetPosition(const Pht::Vec3& position);
        
    private:
        MapScene& mMapScene;
        std::unique_ptr<Pht::SceneObject> mSceneObject;
        Pht::SceneResources mSceneResources;
    };
}

#endif
