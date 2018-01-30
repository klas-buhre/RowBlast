#ifndef GradientRectangle_hpp
#define GradientRectangle_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class Material;
    class SceneResources;
}

namespace BlocksGame {
    struct GradientRectangleColors {
        Pht::Vec4 mLeft;
        Pht::Vec4 mMid;
        Pht::Vec4 mRight;
    };
    
    Pht::SceneObject& CreateGradientRectangle(Pht::Scene& scene,
                                              Pht::SceneObject& parentObject,
                                              const Pht::Vec3& position,
                                              const Pht::Vec2& size,
                                              float tilt,
                                              float leftQuadWidth,
                                              float rightQuadWidth,
                                              const GradientRectangleColors& upperColors,
                                              const GradientRectangleColors& lowerColors);
}

#endif
