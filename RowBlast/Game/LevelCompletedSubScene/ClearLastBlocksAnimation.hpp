#ifndef ClearLastBlocksAnimation_hpp
#define ClearLastBlocksAnimation_hpp

namespace RowBlast {
    class FlyingBlocksAnimation;
    class Field;
    
    class ClearLastBlocksAnimation {
    public:
        enum class State {
            BeforeClear,
            Ongoing,
            Inactive
        };
        
        ClearLastBlocksAnimation(Field& field, FlyingBlocksAnimation& flyingBlocksAnimation);
    
        void Start();
        State Update(float dt);
        
    private:
        void UpdateInBeforeClearState(float dt);
        void UpdateInOngoingState(float dt);
        
        Field& mField;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
    };
}

#endif