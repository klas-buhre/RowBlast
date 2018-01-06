#ifndef Chapter_hpp
#define Chapter_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "FloatingCubes.hpp"

namespace BlocksGame {
    struct MapLevel {
        int mLevelIndex;
        Pht::Vec3 mPosition;
    };

    class Chapter {
    public:
        Chapter(const std::vector<MapLevel>& levels, const std::vector<CubePathVolume>& cubePaths);
        
        const std::vector<MapLevel>& mLevels;
        const std::vector<CubePathVolume>& mCubePaths;
    };
    
    Chapter GetChapter(int chapterIndex);
}

#endif
