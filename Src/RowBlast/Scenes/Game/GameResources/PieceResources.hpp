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

namespace RowBlast {
    class CommonResources;
    
    enum class BondRenderableKind {
        Normal,
        Aslope,
        Diagonal
    };
    
    class PieceResources {
    public:
        PieceResources(Pht::IEngine& engine, const CommonResources& commonResources);
        
        Pht::RenderableObject& GetBlockRenderableObject(BlockKind blockKind,
                                                        BlockColor color,
                                                        BlockBrightness brightness) const;
        Pht::RenderableObject& GetBondRenderableObject(BondRenderableKind bondRenderableKind,
                                                       BlockColor color,
                                                       BlockBrightness brightness) const;
        Pht::RenderableObject& GetPreviewAslopeBondRenderableObject(BlockColor color) const;

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
        int CalcBlockIndex(BlockKind blockKind,
                           BlockColor color,
                           BlockBrightness brightness) const;
        int CalcBondIndex(BondRenderableKind bondRenderableKind,
                          BlockColor color,
                          BlockBrightness brightness) const;
        void CreateBlocks(Pht::ISceneManager& sceneManager, const CommonResources& commonResources);
        void CreateBonds(Pht::ISceneManager& sceneManager, const CommonResources& commonResources);
        void CreatePreviewAslopeBonds(Pht::ISceneManager& sceneManager,
                                      const CommonResources& commonResources);
        void CreateBombs(Pht::ISceneManager& sceneManager);
        
        std::vector<std::unique_ptr<Pht::RenderableObject>> mBlocks;
        std::vector<std::unique_ptr<Pht::RenderableObject>> mBonds;
        std::vector<std::unique_ptr<Pht::RenderableObject>> mPreviewAslopeBonds;
        std::unique_ptr<Pht::RenderableObject> mBomb;
        std::unique_ptr<Pht::RenderableObject> mTransparentBomb;
        std::unique_ptr<Pht::RenderableObject> mRowBomb;
        std::unique_ptr<Pht::RenderableObject> mTransparentRowBomb;
    };
}

#endif
