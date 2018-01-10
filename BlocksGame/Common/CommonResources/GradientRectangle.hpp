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

    class GradientRectangle {
    public:
        GradientRectangle(Pht::IEngine& engine,
                          Pht::SceneResources& sceneResources,
                          const Pht::Vec3& position,
                          const Pht::Vec2& size,
                          float tilt,
                          float leftQuadWidth,
                          float rightQuadWidth,
                          const GradientRectangleColors& upperColors,
                          const GradientRectangleColors& lowerColors);
        
        const Pht::SceneObject& GetMidQuad() const {
            return *mMidQuad;
        }

        const Pht::SceneObject& GetLeftQuad() const {
            return *mLeftQuad;
        }

        const Pht::SceneObject& GetRightQuad() const {
            return *mRightQuad;
        }
        
    private:
        std::unique_ptr<Pht::SceneObject> CreateGradientQuad(const Pht::Vec2& size,
                                                             float tilt,
                                                             const Pht::Vec4& leftUpperColor,
                                                             const Pht::Vec4& rightUpperColor,
                                                             const Pht::Vec4& leftLowerColor,
                                                             const Pht::Vec4& rightLowerColor,
                                                             const Pht::Material& material,
                                                             Pht::IEngine& engine,
                                                             Pht::SceneResources& sceneResources);

        std::unique_ptr<Pht::SceneObject> mMidQuad;
        std::unique_ptr<Pht::SceneObject> mLeftQuad;
        std::unique_ptr<Pht::SceneObject> mRightQuad;
    };
}

#endif
