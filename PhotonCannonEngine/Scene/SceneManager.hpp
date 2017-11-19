#ifndef SceneManager_hpp
#define SceneManager_hpp

#include "ISceneManager.hpp"

namespace Pht {
    class Renderer;
    
    class SceneManager: public ISceneManager {
    public:
        SceneManager(Renderer& renderer);
        ~SceneManager();
        
        void SetLoadedScene(std::unique_ptr<Scene> scene) override;
        Scene* GetScene() override;
        std::unique_ptr<RenderableObject> CreateRenderableObject(const IMesh& mesh,
                                                                 const Material& material) override;
        std::unique_ptr<SceneObject> CreateCamera() override;
        std::unique_ptr<SceneObject> CreateText(const std::string& text,
                                                const TextProperties& properties) override;

    private:
        Renderer& mRenderer;
        std::unique_ptr<Scene> mScene;
    };
}

#endif
