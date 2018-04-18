#ifndef Chapter_hpp
#define Chapter_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "FloatingBlocks.hpp"

namespace RowBlast {
    struct MapLevel {
        int mLevelIndex;
        Pht::Vec3 mPosition;
    };

    class Chapter {
    public:
        Chapter(const std::vector<MapLevel>& levels, const std::vector<BlockPathVolume>& blockPaths);
        
        const std::vector<MapLevel>& mLevels;
        const std::vector<BlockPathVolume>& mBlockPaths;
    };
    
    Chapter GetChapter(int chapterIndex);
}

#endif
