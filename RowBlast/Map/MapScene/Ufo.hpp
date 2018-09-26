#ifndef Ufo_hpp
#define Ufo_hpp

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
    
    class Ufo {
    public:
        Ufo(Pht::IEngine& engine, MapScene& mapScene, const CommonResources& commonResources);
        
        void Init();
        void SetPosition(const Pht::Vec3& position);
        const Pht::Vec3& GetPosition() const;
        void SetRotation(const Pht::Vec3& rotation);
        void SetHoverTranslation(float hoverTranslation);
        void Hide();
        void Show();
        
    private:
        MapScene& mMapScene;
        std::unique_ptr<Pht::SceneObject> mSceneObject;
        std::unique_ptr<Pht::SceneObject> mUfoSceneObject;
        Pht::SceneResources mSceneResources;
    };
}

#endif
