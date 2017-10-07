#ifndef ClearLastBlocksAnimation_hpp
#define ClearLastBlocksAnimation_hpp

namespace BlocksGame {
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
        
        State mState {State::Inactive};
        Field& mField;
        FlyingBlocksAnimation& mFlyingBlocksAnimation;
        float mElapsedTime {0.0f};
    };
}

#endif
