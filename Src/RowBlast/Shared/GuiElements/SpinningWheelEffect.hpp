#ifndef SpinningWheelEffect_hpp
#define SpinningWheelEffect_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class SpinningWheelEffect {
    public:
        SpinningWheelEffect(Pht::IEngine& engine);
        
        void Init(Pht::SceneObject& parentObject);
        void Start();
        void Stop();
        void Update();
        
    private:
        Pht::IEngine& mEngine;
        std::unique_ptr<Pht::SceneObject> mSceneObject;
    };
}

#endif
