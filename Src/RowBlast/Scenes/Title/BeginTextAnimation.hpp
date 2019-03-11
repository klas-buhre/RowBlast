#ifndef BeginTextAnimation_hpp
#define BeginTextAnimation_hpp

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
}

namespace RowBlast {
    class CommonResources;
    
    class BeginTextAnimation {
    public:
        BeginTextAnimation(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Init(Pht::Scene& scene, Pht::SceneObject& parentObject);
        void Start();
        bool IsActive() const;

    private:
        Pht::IEngine& mEngine;
        const CommonResources& mCommonResources;
        Pht::SceneObject* mSceneObject {nullptr};
    };
}

#endif
