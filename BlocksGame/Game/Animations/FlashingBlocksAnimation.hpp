#ifndef FlashingBlocksAnimation_hpp
#define FlashingBlocksAnimation_hpp

// Engine includes.
#include "Material.hpp"

// Game includes.
#include "Cell.hpp"

namespace RowBlast {
    class Field;
    
    class FlashingBlocksAnimation {
    public:
        FlashingBlocksAnimation(Field& field);
    
        void Update(float dt);
        
        static const Pht::Color colorAdd;
        static const Pht::Color brightColorAdd;
        static const Pht::Color semiFlashingColorAdd;
        
    private:
        void AnimateFlashingBlock(SubCell& subCell, int row, int column, float dt);
        bool IsBlockAccordingToBlueprint(SubCell& subCell, int row, int column);
        
        Field& mField;
    };
}

#endif
