#ifndef GradientRectangle_hpp
#define GradientRectangle_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class IEngine;
    class Material;
}

namespace BlocksGame {
    class GradientRectangle {
    public:
        struct Colors {
            Pht::Vec4 mLeft;
            Pht::Vec4 mMid;
            Pht::Vec4 mRight;
        };
        
        GradientRectangle(Pht::IEngine& engine,
                          const Pht::Vec3& position,
                          const Pht::Vec2& size,
                          float tilt,
                          float leftQuadWidth,
                          float rightQuadWidth,
                          const Colors& upperColors,
                          const Colors& lowerColors);
        
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
        std::unique_ptr<Pht::RenderableObject> CreateGradientQuad(const Pht::Vec2& size,
                                                                  float tilt,
                                                                  const Pht::Vec4& leftUpperColor,
                                                                  const Pht::Vec4& rightUpperColor,
                                                                  const Pht::Vec4& leftLowerColor,
                                                                  const Pht::Vec4& rightLowerColor,
                                                                  const Pht::Material& material,
                                                                  Pht::IEngine& engine);

        std::unique_ptr<Pht::SceneObject> mMidQuad;
        std::unique_ptr<Pht::SceneObject> mLeftQuad;
        std::unique_ptr<Pht::SceneObject> mRightQuad;
    };
}

#endif
