#ifndef WeldsAnimation_hpp
#define WeldsAnimation_hpp

namespace BlocksGame {
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
        void AnimateBlockWelds(SubCell& subCell, float dt);
        void AnimateWeld(WeldAnimation& animation, float dt);
        void AnimateWeldAppearing(WeldAnimation& animation, float dt);
        void AnimateWeldDisappearing(WeldAnimation& animation, float dt);
        
        Field& mField;
    };
}

#endif
