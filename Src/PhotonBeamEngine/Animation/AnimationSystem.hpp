#ifndef AnimationSystem_hpp
#define AnimationSystem_hpp

#include <vector>

#include "IAnimationSystem.hpp"

namespace Pht {
    class AnimationSystem: public IAnimationSystem {
    public:
        void AddAnimation(Animation& animation) override;
        void RemoveAnimation(Animation& animation) override;
        Animation& CreateAnimation(SceneObject& sceneObject) override;

        void Update(float dt);
        
    private:
        std::vector<Animation*> mAnimations;
    };
}

#endif
