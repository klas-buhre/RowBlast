#ifndef ISceneObjectComponent_hpp
#define ISceneObjectComponent_hpp

#include <cstdint>

namespace Pht {
    class SceneObject;
    
    using ComponentId = uint32_t;
    
    class ISceneObjectComponent {
    public:
        virtual ~ISceneObjectComponent() {}
        
        virtual SceneObject& GetSceneObject() = 0;
    };
}

#endif
