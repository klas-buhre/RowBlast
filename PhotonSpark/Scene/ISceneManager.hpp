#ifndef ISceneManager_hpp
#define ISceneManager_hpp

#include <memory>

#include "Scene.hpp"

namespace Pht {
    class RenderableObject;
    class IMesh;
    class Material;
    class SceneObject;

    class ISceneManager {
    public:
        virtual ~ISceneManager() {}
        
        virtual std::unique_ptr<Scene> CreateScene(Scene::Name name) = 0;
        virtual void SetLoadedScene(std::unique_ptr<Scene> scene) = 0;
        virtual Scene* GetActiveScene() = 0;
        virtual std::unique_ptr<RenderableObject> CreateRenderableObject(const IMesh& mesh,
                                                                         const Material& material) = 0;
        virtual std::unique_ptr<SceneObject> CreateSceneObject(const IMesh& mesh,
                                                               const Material& material) = 0;
    };
}

#endif
