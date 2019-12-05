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
    
    enum class WeldRenderableKind {
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
        Pht::RenderableObject& GetWeldRenderableObject(WeldRenderableKind weldRenderableKind,
                                                       BlockColor color,
                                                       BlockBrightness brightness) const;
        Pht::RenderableObject& GetPreviewAslopeWeldRenderableObject(BlockColor color) const;

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
        int CalcWeldIndex(WeldRenderableKind weldRenderableKind,
                          BlockColor color,
                          BlockBrightness brightness) const;
        void CreateBlocks(Pht::ISceneManager& sceneManager, const CommonResources& commonResources);
        void CreateWelds(Pht::ISceneManager& sceneManager, const CommonResources& commonResources);
        void CreatePreviewAslopeWelds(Pht::ISceneManager& sceneManager,
                                      const CommonResources& commonResources);
        void CreateBombs(Pht::ISceneManager& sceneManager);
        
        std::vector<std::unique_ptr<Pht::RenderableObject>> mBlocks;
        std::vector<std::unique_ptr<Pht::RenderableObject>> mWelds;
        std::vector<std::unique_ptr<Pht::RenderableObject>> mPreviewAslopeWelds;
        std::unique_ptr<Pht::RenderableObject> mBomb;
        std::unique_ptr<Pht::RenderableObject> mTransparentBomb;
        std::unique_ptr<Pht::RenderableObject> mRowBomb;
        std::unique_ptr<Pht::RenderableObject> mTransparentRowBomb;
    };
}

#endif
