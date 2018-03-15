#ifndef CollapsingFieldAnimation_hpp
#define CollapsingFieldAnimation_hpp

namespace BlocksGame {
    class Field;
    
    class CollapsingFieldAnimation {
    public:
        enum class State {
            Waiting,
            Active,
            Inactive
        };
        
        explicit CollapsingFieldAnimation(Field& field);
    
        State Update(float dt);
        
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
