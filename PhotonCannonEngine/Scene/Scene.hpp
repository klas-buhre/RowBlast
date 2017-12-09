#ifndef Scene_hpp
#define Scene_hpp

#include <memory>
#include <vector>
#include <string>

#include "Vector.hpp"
#include "RenderPass.hpp"

namespace Pht {
    class SceneObject;
    class LightComponent;
    class CameraComponent;
    class TextComponent;
    class TextProperties;
    class ISceneManager;
    class IMesh;
    class Material;
    
    enum class DistanceFunction {
        CameraSpaceZ,
        WorldSpaceZ
    };
    
    class Scene {
    public:
        using Name = uint32_t;
        
        Scene(ISceneManager& sceneManager, Name name);
        ~Scene();
        
        SceneObject& GetRoot();
        const SceneObject& GetRoot() const;
        LightComponent& CreateGlobalLight();
        CameraComponent& CreateCamera();
        SceneObject& CreateSceneObject(const IMesh& mesh, const Material& material);
        TextComponent& CreateText(const std::string& text, const TextProperties& properties);
        void AddSceneObject(std::unique_ptr<SceneObject> sceneObject);
        void AddRenderPass(const RenderPass& renderPass);
        
        LightComponent* GetGlobalLight() {
            return mGlobalLight;
        }
        
        const LightComponent* GetGlobalLight() const {
            return mGlobalLight;
        }
        
        CameraComponent* GetCamera() {
            return mCamera;
        }
        
        const CameraComponent* GetCamera() const {
            return mCamera;
        }
        
        Name GetName() const {
            return mName;
        }

        void SetDistanceFunction(DistanceFunction distanceFunction) {
            mDistanceFunction = distanceFunction;
        }
        
        DistanceFunction GetDistanceFunction() const {
            return mDistanceFunction;
        }
        
        const std::vector<RenderPass>& GetRenderPasses() const {
            return mRenderPasses;
        }

    private:
        ISceneManager& mSceneManager;
        Name mName {0};
        std::vector<std::unique_ptr<SceneObject>> mSceneObjects;
        SceneObject* mRoot {nullptr};
        LightComponent* mGlobalLight {nullptr};
        CameraComponent* mCamera {nullptr};
        DistanceFunction mDistanceFunction {DistanceFunction::CameraSpaceZ};
        std::vector<RenderPass> mRenderPasses;
    };
}

#endif
