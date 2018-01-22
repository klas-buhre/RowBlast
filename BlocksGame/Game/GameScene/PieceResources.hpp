#ifndef PieceResources_hpp
#define PieceResources_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "RenderableObject.hpp"

// Game includes.
#include "Cell.hpp"

namespace Pht {
    class IEngine;
    class ISceneManager;
}

namespace BlocksGame {
    class GameScene;
    
    class PieceResources {
    public:
        PieceResources(Pht::IEngine& engine, const GameScene& scene);
        
        Pht::RenderableObject& GetBlockRenderableObject(BlockRenderableKind blockRenderableKind,
                                                        BlockColor color,
                                                        BlockBrightness brightness) const;
        Pht::RenderableObject& GetWeldRenderableObject(BlockColor color,
                                                       BlockBrightness brightness) const;

    private:
        int CalcBlockIndex(BlockRenderableKind blockRenderableKind,
                           BlockColor color,
                           BlockBrightness brightness) const ;
        int CalcWeldIndex(BlockColor color, BlockBrightness brightness) const;
        void CreateBlocks(Pht::ISceneManager& sceneManager, const GameScene& scene);
        void CreateWelds(Pht::ISceneManager& sceneManager, const GameScene& scene);
        
        std::vector<std::unique_ptr<Pht::RenderableObject>> mBlocks;
        std::vector<std::unique_ptr<Pht::RenderableObject>> mWelds;
    };
}

#endif
