#ifndef TitleScene_hpp
#define TitleScene_hpp

#include <memory>

// Game includes.
#include "FloatingBlocks.hpp"
#include "Clouds.hpp"
#include "Planets.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
}

namespace RowBlast {
    class CommonResources;
    class UserServices;
    class Universe;
    
    class TitleScene {
    public:
        TitleScene(Pht::IEngine& engine,
                   const CommonResources& commonResources,
                   const UserServices& userServices,
                   const Universe& universe);
        
        void Init();
        void Update();
        
        Pht::SceneObject& GetUfoContainer() {
            assert(mUfoContainer);
            return *mUfoContainer;
        }
        
        Pht::SceneObject& GetUiContainer() {
            assert(mUiContainer);
            return *mUiContainer;
        }

        Pht::Scene& GetScene() {
            assert(mScene);
            return *mScene;
        }
        
    private:
        Pht::IEngine& mEngine;
        const CommonResources& mCommonResources;
        const UserServices& mUserServices;
        const Universe& mUniverse;
        std::unique_ptr<Planets> mPlanets;
        std::unique_ptr<Clouds> mClouds;
        std::unique_ptr<FloatingBlocks> mFloatingBlocks;
        Pht::SceneObject* mUfoContainer {nullptr};
        Pht::SceneObject* mUiContainer {nullptr};
        Pht::Scene* mScene {nullptr};
    };
}

#endif
