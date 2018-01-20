#ifndef FlashingBlocksAnimation_hpp
#define FlashingBlocksAnimation_hpp

// Engine includes.
#include "Material.hpp"

// Game includes.
#include "Cell.hpp"

namespace BlocksGame {
    class Field;
    
    class FlashingBlocksAnimation {
    public:
        FlashingBlocksAnimation(Field& field);
    
        void Update(float dt);
        
        static const Pht::Color colorAdd;
        static const Pht::Color brightColorAdd;
        
    private:
        void AnimateFlashingBlock(SubCell& subCell, int row, int column, float dt);
        bool IsBlockAccordingToBlueprint(SubCell& subCell, int row, int column);
        
        Field& mField;
    };
}

#endif
