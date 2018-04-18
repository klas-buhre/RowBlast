#ifndef SceneManager_hpp
#define SceneManager_hpp

#include "ISceneManager.hpp"

namespace Pht {
    class Renderer;
    
    class SceneManager: public ISceneManager {
    public:
        SceneManager(Renderer& renderer);
        ~SceneManager();
        
        std::unique_ptr<Scene> CreateScene(Scene::Name name) override;
        void SetLoadedScene(std::unique_ptr<Scene> scene) override;
        Scene* GetActiveScene() override;
        std::unique_ptr<RenderableObject> CreateRenderableObject(const IMesh& mesh,
                                                                 const Material& material) override;
        std::unique_ptr<SceneObject> CreateSceneObject(const IMesh& mesh,
                                                       const Material& material,
                                                       SceneResources& sceneResources) override;

    private:
        Renderer& mRenderer;
        std::unique_ptr<Scene> mScene;
    };
}

#endif
