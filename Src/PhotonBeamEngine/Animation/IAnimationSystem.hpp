#ifndef IAnimationSystem_hpp
#define IAnimationSystem_hpp

namespace Pht {
    class Animation;
    class SceneObject;
    
    class IAnimationSystem {
    public:
        virtual ~IAnimationSystem() {}

        virtual void AddAnimation(Animation& animation) = 0;
        virtual void RemoveAnimation(Animation& animation) = 0;
        virtual Animation& CreateAnimation(SceneObject& sceneObject) = 0;
    };
}

#endif
