#ifndef MenuQuad_hpp
#define MenuQuad_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class SceneResources;
}

namespace BlocksGame {
    namespace MenuQuad {
        std::unique_ptr<Pht::SceneObject> CreateGray(Pht::IEngine& engine,
                                                     Pht::SceneResources& sceneResources,
                                                     const Pht::Vec2& size);
        std::unique_ptr<Pht::SceneObject> CreateGreen(Pht::IEngine& engine,
                                                      Pht::SceneResources& sceneResources,
                                                      const Pht::Vec2& size);
    }
}

#endif
