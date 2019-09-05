#ifndef TutorialFieldBottomGlow_hpp
#define TutorialFieldBottomGlow_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"
#include "RenderableObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class TutorialFieldBottomGlow {
    public:
        TutorialFieldBottomGlow(Pht::IEngine& engine, Pht::SceneObject& parent);

        void Update(float dt);
    
    private:
        float mElapsedTime {0.0f};
        std::unique_ptr<Pht::SceneObject> mGlowSceneObject;
        std::unique_ptr<Pht::RenderableObject> mGlowRenderable;
    };
}

#endif
