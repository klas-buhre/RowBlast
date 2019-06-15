#ifndef SceneManager_hpp
#define SceneManager_hpp

#include "ISceneManager.hpp"

namespace Pht {
    class IRenderSystem;
    class InputHandler;
    
    class SceneManager: public ISceneManager {
    public:
        SceneManager(IRenderSystem& renderer, InputHandler& inputHandler);
        ~SceneManager();
        
        std::unique_ptr<Scene> CreateScene(Scene::Name name) override;
        void InitSceneSystems(float narrowFrustumHeightFactor) override;
        void InitRenderer() override;
        void SetLoadedScene(std::unique_ptr<Scene> scene) override;
        Scene* GetActiveScene() override;
        std::unique_ptr<RenderableObject> CreateRenderableObject(const IMesh& mesh,
                                                                 const Material& material) override;
        std::unique_ptr<RenderableObject> CreateBatchableRenderableObject(const IMesh& mesh,
                                                                          const Material& material) override;
        std::unique_ptr<RenderableObject> CreateStaticBatchRenderable(const SceneObject& sceneObject,
                                                                      const Optional<std::string>& batchVertexBufferName) override;
        std::unique_ptr<SceneObject> CreateSceneObject(const IMesh& mesh,
                                                       const Material& material,
                                                       SceneResources& sceneResources) override;

    private:
        IRenderSystem& mRenderer;
        InputHandler& mInputHandler;
        std::unique_ptr<Scene> mScene;
    };
}

#endif
