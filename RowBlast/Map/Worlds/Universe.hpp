#ifndef Universe_hpp
#define Universe_hpp

#include <vector>

// Game includes.
#include "World.hpp"

namespace RowBlast {
    class Universe {
    public:
        Universe();
        
        const World& GetWorld(int worldIndex) const;
        
    private:
        std::vector<World> mWorlds;
    };
}

#endif
