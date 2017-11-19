#ifndef ISceneManager_hpp
#define ISceneManager_hpp

#include <memory>

namespace Pht {
    class Scene;
    class RenderableObject;
    class IMesh;
    class Material;
    class SceneObject;
    class TextProperties;

    class ISceneManager {
    public:
        virtual ~ISceneManager() {}
        
        virtual void SetLoadedScene(std::unique_ptr<Scene> scene) = 0;
        virtual Scene* GetScene() = 0;
        virtual std::unique_ptr<RenderableObject> CreateRenderableObject(const IMesh& mesh,
                                                                         const Material& material) = 0;
        virtual std::unique_ptr<SceneObject> CreateCamera() = 0;
        virtual std::unique_ptr<SceneObject> CreateText(const std::string& text,
                                                        const TextProperties& properties) = 0;
    };
}

#endif
