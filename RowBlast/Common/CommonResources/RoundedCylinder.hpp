#ifndef RoundedCylinder_hpp
#define RoundedCylinder_hpp

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class Scene;
    class SceneObject;
    class Color;
}

namespace RowBlast {
    Pht::SceneObject& CreateRoundedCylinder(Pht::Scene& scene,
                                            Pht::SceneObject& parentObject,
                                            const Pht::Vec3& position,
                                            const Pht::Vec2& size,
                                            float opacity,
                                            const Pht::Color& ambient,
                                            const Pht::Color& diffuse);
}

#endif
