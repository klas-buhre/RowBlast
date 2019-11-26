#ifndef CollapsingFieldAnimationSystem_hpp
#define CollapsingFieldAnimationSystem_hpp

namespace RowBlast {
    class Field;
    
    class CollapsingFieldAnimationSystem {
    public:
        enum class State {
            Waiting,
            Active,
            BlocksBouncing,
            Inactive
        };
        
        explicit CollapsingFieldAnimationSystem(Field& field);
    
        State Update(float dt);
        void ResetBlockAnimations();
        void TransitionWronglyBouncingBlocksToFalling();
        
        void GoToBlocksBouncingState() {
            mState = State::BlocksBouncing;
        }
        
        void GoToInactiveState() {
            mState = State::Inactive;
        }
        
        bool IsInactive() const {
            return mState == State::Inactive;
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
