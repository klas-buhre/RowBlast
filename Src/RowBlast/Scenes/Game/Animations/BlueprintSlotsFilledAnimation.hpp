#ifndef SlotsFilledAnimationSystem_hpp
#define SlotsFilledAnimationSystem_hpp

#include <array>

// Engine includes.
#include "SceneResources.hpp"

namespace Pht {
    class SceneObject;
}

namespace RowBlast {
    class Field;
    class GameScene;
    class LevelResources;
    class SlotFillAnimationComponent;
    
    class SlotsFilledAnimationSystem {
    public:
        SlotsFilledAnimationSystem(Field& field,
                                   GameScene& scene,
                                   const LevelResources& levelResources);
    
        void Init();
        void Update(float dt);
        
    private:
        void AnimateSlot(int row,
                         int column,
                         SlotFillAnimationComponent& animation,
                         float dt);
        
        static constexpr int numSceneObjects {10};
        
        Field& mField;
        GameScene& mScene;
        Pht::SceneResources mSceneResources;
        Pht::SceneObject* mContainerSceneObject {nullptr};
        std::array<Pht::SceneObject*, numSceneObjects> mSceneObjects;
        int mNextAvailableSceneObject {0};
    };
}

#endif
