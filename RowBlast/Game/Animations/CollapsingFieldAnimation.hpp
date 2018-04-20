#ifndef CollapsingFieldAnimation_hpp
#define CollapsingFieldAnimation_hpp

namespace RowBlast {
    class Field;
    
    class CollapsingFieldAnimation {
    public:
        enum class State {
            Waiting,
            Active,
            BlocksBouncing,
            Inactive
        };
        
        explicit CollapsingFieldAnimation(Field& field);
    
        State Update(float dt);
        void ResetBlockAnimations();
        
        void GoToBlocksBouncingState() {
            mState = State::BlocksBouncing;
        }
        
        void GoToInactiveState() {
            mState = State::Inactive;
        }

    private:
        void UpdateInWaitingState(float dt);
        void UpdateInActiveState(float dt);
        void UpdateInInactiveState();
        void GoToWaitingState();
        
        Field& mField;
        State mState {State::Inactive};
        float mWaitedTime {0.0f};
    };
}

#endif
