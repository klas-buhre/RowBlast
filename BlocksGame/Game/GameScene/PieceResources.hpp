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
    
    enum class WeldRenderableKind {
        Normal,
        Aslope,
        Diagonal
    };
    
    class PieceResources {
    public:
        PieceResources(Pht::IEngine& engine, const GameScene& scene);
        
        Pht::RenderableObject& GetBlockRenderableObject(BlockRenderableKind blockRenderableKind,
                                                        BlockColor color,
                                                        BlockBrightness brightness) const;
        Pht::RenderableObject& GetWeldRenderableObject(WeldRenderableKind weldRenderableKind,
                                                       BlockColor color,
                                                       BlockBrightness brightness) const;

        Pht::RenderableObject& GetBombRenderableObject() const {
            return *mBomb;
        }

        Pht::RenderableObject& GetTransparentBombRenderableObject() const {
            return *mTransparentBomb;
        }

        Pht::RenderableObject& GetRowBombRenderableObject() const {
            return *mRowBomb;
        }

        Pht::RenderableObject& GetTransparentRowBombRenderableObject() const {
            return *mTransparentRowBomb;
        }

    private:
        int CalcBlockIndex(BlockRenderableKind blockRenderableKind,
                           BlockColor color,
                           BlockBrightness brightness) const ;
        int CalcWeldIndex(WeldRenderableKind weldRenderableKind,
                          BlockColor color,
                          BlockBrightness brightness) const;
        void CreateBlocks(Pht::ISceneManager& sceneManager, const GameScene& scene);
        void CreateWelds(Pht::ISceneManager& sceneManager, const GameScene& scene);
        void CreateBombs(Pht::ISceneManager& sceneManager, const GameScene& scene);
        
        std::vector<std::unique_ptr<Pht::RenderableObject>> mBlocks;
        std::vector<std::unique_ptr<Pht::RenderableObject>> mWelds;
        std::unique_ptr<Pht::RenderableObject> mBomb;
        std::unique_ptr<Pht::RenderableObject> mTransparentBomb;
        std::unique_ptr<Pht::RenderableObject> mRowBomb;
        std::unique_ptr<Pht::RenderableObject> mTransparentRowBomb;
    };
}

#endif
