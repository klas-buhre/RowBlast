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
        void Update();
        bool IsActive() const;

    private:
        void UpdateInTextVisibleState();
        void UpdateInTextInvisibleState();
        void GoToTextVisibleState();
        void GoToTextInvisibleState();
    
        enum class State {
            TextVisible,
            TextInvisible,
            Inactive
        };

        Pht::IEngine& mEngine;
        const CommonResources& mCommonResources;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
        Pht::SceneObject* mSceneObject {nullptr};
    };
}

#endif
