#ifndef BlastArea_hpp
#define BlastArea_hpp

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
    class CommonResources;
    
    class BlastArea {
    public:
        enum class Kind {
            Bomb,
            BigBomb
        };

        BlastArea(Pht::IEngine& engine,
                             GameScene& scene,
                             const CommonResources& commonResources);
    
        void Init();
        void Start(Kind kind);
        void Stop();
        void SetPosition(const Pht::Vec2& position);
        void Update(float dt);
        bool IsActive() const;
        Pht::RenderableObject& GetBombRadiusRenderable() const;

        Kind GetActiveKind() const {
            return mActiveKind;
        }
        
    private:
        std::unique_ptr<Pht::SceneObject> CreateSceneObject(const Pht::IImage& image,
                                                            const std::string& imageName,
                                                            float squareSide,
                                                            Pht::IEngine& engine);
        std::unique_ptr<Pht::SceneObject> CreateShadowSceneObject(const Pht::IImage& image,
                                                                  const std::string& imageName,
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
        std::unique_ptr<Pht::SceneObject> mBombBlastAreaContainer;
        std::unique_ptr<Pht::SceneObject> mBombBlastAreaSceneObject;
        std::unique_ptr<Pht::SceneObject> mBombBlastAreaShadowSceneObject;
        std::unique_ptr<Pht::SceneObject> mBigBombBlastAreaContainer;
        std::unique_ptr<Pht::SceneObject> mBigBombBlastAreaSceneObject;
        std::unique_ptr<Pht::SceneObject> mBigBombBlastAreaShadowSceneObject;
        float mTime {0.0f};
        Pht::SceneResources mSceneResources;
    };
}

#endif
