#ifndef PieceResources_hpp
#define PieceResources_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "RenderableObject.hpp"

// Game includes.
#include "Cell.hpp"

namespace Pht {
    class ISceneManager;
}

namespace BlocksGame {
    class GameScene;
    
    class PieceResources {
    public:
        PieceResources(Pht::ISceneManager& sceneManager, const GameScene& scene);
        
        Pht::RenderableObject& GetBlockRenderableObject(BlockRenderable blockRenderable,
                                                        BlockColor color,
                                                        BlockBrightness brightness);
        Pht::RenderableObject& GetWeldRenderableObject(BlockColor color,
                                                       BlockBrightness brightness);

    private:
        void CreateBlocks(Pht::ISceneManager& sceneManager, const GameScene& scene);
        void CreateWelds(Pht::ISceneManager& sceneManager, const GameScene& scene);
        
        std::vector<std::vector<std::vector<std::unique_ptr<Pht::RenderableObject>>>> mBlocks;
        std::vector<std::vector<std::unique_ptr<Pht::RenderableObject>>> mWelds;
    };
}

#endif
