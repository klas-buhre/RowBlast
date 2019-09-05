#ifndef GhostPieceBlocks_hpp
#define GhostPieceBlocks_hpp

// Engine includes.
#include "RenderableObject.hpp"

// Game includes.
#include "Cell.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources;
    
    class GhostPieceBlocks {
    public:
        GhostPieceBlocks(Pht::IEngine& engine, const CommonResources& commonResources);

        Pht::RenderableObject& GetBlockRenderableObject(BlockKind blockKind,
                                                        BlockColor color) const;

    private:
        int CalcBlockIndex(BlockKind blockKind, BlockColor color) const;

        std::vector<std::unique_ptr<Pht::RenderableObject>> mBlocks;
    };
}

#endif
