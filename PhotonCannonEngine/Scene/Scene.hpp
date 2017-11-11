#ifndef Scene_hpp
#define Scene_hpp

#include <memory>

#include "Vector.hpp"

namespace Pht {
    class SceneObject;
    
    class Scene {
    public:
        Scene();
        ~Scene();
        
        SceneObject& GetRoot();
        const SceneObject& GetRoot() const;
        void SetCamera(std::unique_ptr<SceneObject> camera);
        SceneObject& GetCamera();
        const SceneObject& GetCamera() const;

        void SetLightDirection(const Pht::Vec3& lightDirection) {
            mLightDirection = lightDirection;
        }
        
        const Vec3& GetLightDirection() const {
            return mLightDirection;
        }

    private:
        std::unique_ptr<SceneObject> mRoot;
        Vec3 mLightDirection {1.0f, 1.0f, 1.0f};
        std::unique_ptr<SceneObject> mCamera;
    };
}

#endif
