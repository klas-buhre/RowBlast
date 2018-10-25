#ifndef LevelLoader_hpp
#define LevelLoader_hpp

// Game includes.
#include "Level.hpp"

namespace RowBlast {
    class LevelResources;
    
    namespace LevelLoader {
        std::unique_ptr<Level> Load(int levelId, const LevelResources& levelResources);
        std::unique_ptr<LevelInfo> LoadInfo(int levelId, const LevelResources& levelResources);
    }
}

#endif
