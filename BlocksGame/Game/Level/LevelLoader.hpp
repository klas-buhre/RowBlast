#ifndef LevelLoader_hpp
#define LevelLoader_hpp

// Game includes.
#include "Level.hpp"

namespace RowBlast {
    class LevelResources;
    
    namespace LevelLoader {
        std::unique_ptr<Level> Load(int levelIndex, const LevelResources& levelResources);
    }
}

#endif
