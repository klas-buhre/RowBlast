#ifndef MenuQuad_hpp
#define MenuQuad_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    namespace MenuQuad {
        std::unique_ptr<Pht::SceneObject> CreateGray(Pht::IEngine& engine, const Pht::Vec2& size);
        std::unique_ptr<Pht::SceneObject> CreateGreen(Pht::IEngine& engine, const Pht::Vec2& size);
    }
}

#endif
