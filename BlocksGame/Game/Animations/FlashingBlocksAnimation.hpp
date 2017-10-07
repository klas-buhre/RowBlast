#ifndef FlashingBlocksAnimation_hpp
#define FlashingBlocksAnimation_hpp

// Game includes.
#include "Cell.hpp"

namespace BlocksGame {
    class Field;
    
    class FlashingBlocksAnimation {
    public:
        FlashingBlocksAnimation(Field& field);
    
        void Update(float dt);
        
    private:
        void AnimateFlashingBlock(SubCell& subCell, int row, int column, float dt);
        bool IsBlockAccordingToBlueprint(SubCell& subCell, int row, int column);
        
        Field& mField;
    };
}

#endif
