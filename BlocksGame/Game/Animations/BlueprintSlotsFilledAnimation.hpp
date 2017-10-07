#ifndef BlueprintSlotsFilledAnimation_hpp
#define BlueprintSlotsFilledAnimation_hpp

namespace BlocksGame {
    class Field;
    
    class BlueprintSlotsFilledAnimation {
    public:
        explicit BlueprintSlotsFilledAnimation(Field& field);
    
        void Update(float dt);
        
    private:
        Field& mField;
    };
}

#endif
