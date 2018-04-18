#ifndef TitleScene_hpp
#define TitleScene_hpp

#include <memory>

// Engine includes.
#include "Font.hpp"

// Game includes.
#include "FloatingBlocks.hpp"
#include "Clouds.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class TitleScene {
    public:
        TitleScene(Pht::IEngine& engine, const CommonResources& commonResources);
        
        void Update();
        
    private:
        std::unique_ptr<Clouds> mClouds;
        std::unique_ptr<FloatingBlocks> mFloatingBlocks;
        Pht::Font mFont;
        Pht::Font mTapFont;
    };
}

#endif
