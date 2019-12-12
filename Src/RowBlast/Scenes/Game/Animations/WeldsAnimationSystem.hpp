#ifndef BondsAnimationSystem_hpp
#define BondsAnimationSystem_hpp

// Engine includes.
#include "Vector.hpp"

namespace RowBlast {
    class Field;
    class SubCell;
    class BondAnimation;
    
    class BondsAnimationSystem {
    public:
        explicit BondsAnimationSystem(Field& field);
    
        void Update(float dt);
        
        static void StartBondAppearingAnimation(BondAnimation& animation);
        static void StartBondDisappearingAnimation(BondAnimation& animation);
        
    private:
        void AnimateBlockBonds(SubCell& subCell, const Pht::IVec2& position, float dt);
        void AnimateUpBond(SubCell& subCell, const Pht::IVec2& position, float dt);
        void AnimateRightBond(SubCell& subCell, const Pht::IVec2& position, float dt);
        void AnimateBondAppearing(BondAnimation& animation, float dt);
        void AnimateBondDisappearing(BondAnimation& animation, float dt);
        void AnimateBond(BondAnimation& animation, bool cellIsFlashing, bool anyBouncing, float dt);
        
        Field& mField;
    };
}

#endif
