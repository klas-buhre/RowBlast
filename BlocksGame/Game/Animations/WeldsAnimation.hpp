#ifndef WeldsAnimation_hpp
#define WeldsAnimation_hpp

// Engine includes.
#include "Vector.hpp"

namespace RowBlast {
    class Field;
    class SubCell;
    class WeldAnimation;
    
    class WeldsAnimation {
    public:
        WeldsAnimation(Field& field);
    
        void Update(float dt);
        
        static void StartWeldAppearingAnimation(WeldAnimation& animation);
        static void StartWeldDisappearingAnimation(WeldAnimation& animation);
        
    private:
        void AnimateBlockWelds(SubCell& subCell, const Pht::IVec2& position, float dt);
        void AnimateUpWeld(SubCell& subCell, const Pht::IVec2& position, float dt);
        void AnimateRightWeld(SubCell& subCell, const Pht::IVec2& position, float dt);
        void AnimateWeldAppearing(WeldAnimation& animation, float dt);
        void AnimateWeldDisappearing(WeldAnimation& animation, float dt);
        void AnimateWeld(WeldAnimation& animation, bool cellIsFlashing, bool anyBouncing, float dt);
        
        Field& mField;
    };
}

#endif
