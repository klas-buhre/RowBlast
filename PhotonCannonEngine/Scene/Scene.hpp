#ifndef Scene_hpp
#define Scene_hpp

#include <memory>

#include "Vector.hpp"
#include "RenderQueue.hpp"

namespace Pht {
    class SceneObject;
    class LightComponent;
    class CameraComponent;
    
    class Scene {
    public:
        using Name = uint32_t;
        
        Scene(Name name);
        ~Scene();
        
        SceneObject& GetRoot();
        const SceneObject& GetRoot() const;
        void SetLight(std::unique_ptr<SceneObject> light);
        void SetCamera(std::unique_ptr<SceneObject> camera);
        LightComponent& GetLight();
        const LightComponent& GetLight() const;
        CameraComponent& GetCamera();
        const CameraComponent& GetCamera() const;
        void SetDistanceFunction(DistanceFunction distanceFunction);
        
        RenderQueue& GetRenderQueue() const {
            return mRenderQueue;
        }
        
        Name GetName() const {
            return mName;
        }

    private:
        Name mName {0};
        std::unique_ptr<SceneObject> mRoot;
        LightComponent* mLight {nullptr};
        CameraComponent* mCamera {nullptr};
        mutable RenderQueue mRenderQueue;
    };
}

#endif
