#ifndef BlastRadiusAnimation_hpp
#define BlastRadiusAnimation_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"
#include "SceneResources.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class GameScene;
    
    class BlastRadiusAnimation {
    public:
        enum class Kind {
            Bomb,
            BigBomb
        };

        BlastRadiusAnimation(Pht::IEngine& engine, GameScene& scene);
    
        void Init();
        void Start(Kind kind);
        void Stop();
        void SetPosition(const Pht::Vec2& position);
        void Update(float dt);
        bool IsActive() const;

        Kind GetActiveKind() const {
            return mActiveKind;
        }
        
    private:
        std::unique_ptr<Pht::SceneObject> CreateSceneObject(const Pht::IImage& image,
                                                            float squareSide,
                                                            Pht::IEngine& engine);
        void UpdateInFadingInState(float dt);
        void UpdateInActiveState(float dt);
        void SetOpacity(float opacity);
        void SetScale(float scale);

        enum class State {
            FadingIn,
            Active,
            Inactive
        };
        
        GameScene& mScene;
        State mState {State::Inactive};
        Kind mActiveKind {Kind::Bomb};
        std::unique_ptr<Pht::SceneObject> mBombRadiusSceneObject;
        std::unique_ptr<Pht::SceneObject> mBigBombRadiusSceneObject;
        float mTime {0.0f};
        Pht::SceneResources mSceneResources;
    };
}

#endif
