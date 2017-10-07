#ifndef MapLoader_hpp
#define MapLoader_hpp

#include <string>
#include <vector>

// Engine includes.
#include "Vector.hpp"

namespace BlocksGame {
    struct MapLevel {
        int mLevelIndex;
        Pht::Vec3 mPosition;
    };
    
    namespace MapLoader {
        void Load(const std::string& filename, std::vector<MapLevel>& levels);
    }
}

#endif
