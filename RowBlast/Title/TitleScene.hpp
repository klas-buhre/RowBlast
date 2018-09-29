#ifndef TitleScene_hpp
#define TitleScene_hpp

#include <memory>

// Engine includes.
#include "Font.hpp"

// Game includes.
#include "FloatingBlocks.hpp"
#include "Clouds.hpp"
#include "Planets.hpp"
#include "TitleAnimation.hpp"

namespace Pht {
    class IEngine;
    class SceneObject;
}

namespace RowBlast {
    class CommonResources;
    
    class TitleScene {
    public:
        TitleScene(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Update();
        
        Pht::SceneObject& GetUfoContainer() {
            assert(mUfoContainer);
            return *mUfoContainer;
        }

    private:
        std::unique_ptr<Planets> mPlanets;
        std::unique_ptr<Clouds> mClouds;
        std::unique_ptr<FloatingBlocks> mFloatingBlocks;
        std::unique_ptr<TitleAnimation> mTitleAnimation;
        Pht::Font mTapFont;
        Pht::SceneObject* mTapTextSceneObject {nullptr};
        Pht::SceneObject* mUfoContainer {nullptr};
    };
}

#endif
