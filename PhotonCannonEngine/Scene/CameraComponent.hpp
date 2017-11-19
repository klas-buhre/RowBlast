#ifndef CameraComponent_hpp
#define CameraComponent_hpp

#include "ISceneObjectComponent.hpp"
#include "Vector.hpp"

namespace Pht {
    class CameraComponent: public ISceneObjectComponent {
    public:
        static const ComponentId id;
        
        CameraComponent(SceneObject& sceneObject);
        
        SceneObject& GetSceneObject() override;
        
        void SetTarget(const Vec3& target, const Vec3& up);
        
        const Vec3& GetTarget() const {
            return mTarget;
        }
        
        const Vec3& GetUp() const {
            return mUp;
        }
        
    private:
        SceneObject& mSceneObject;
        Vec3 mTarget {0.0f, 0.0f, 0.0f};
        Vec3 mUp {0.0f, 1.0f, 0.0f};
    };
}

#endif
