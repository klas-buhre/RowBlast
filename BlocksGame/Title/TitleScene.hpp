#ifndef TitleScene_hpp
#define TitleScene_hpp

#include <memory>

// Engine includes.
#include "SceneObject.hpp"
#include "Font.hpp"

// Game includes.
#include "FloatingCubes.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class CommonResources;
    
    class TitleScene {
    public:
        TitleScene(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Reset();
        void Update();
        const std::vector<FloatingCube>& GetFloatingCubes() const;
        
        const Pht::SceneObject& GetBackground() const {
            return *mBackground;
        }
        
        const Pht::Text& GetTitleText() const {
            return mTitleText;
        }

        const Pht::Text& GetTapText() const {
            return mTapText;
        }
        
    private:
        void CreateBackground(const Pht::Material& backgroundMaterial);

        Pht::IEngine& mEngine;
        std::unique_ptr<Pht::SceneObject> mBackground;
        FloatingCubes mFloatingCubes;
        Pht::Font mFont;
        Pht::Text mTitleText;
        Pht::Font mTapFont;
        Pht::Text mTapText;
    };
}

#endif
