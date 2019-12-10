#ifndef FallingPieceScaleAnimation_hpp
#define FallingPieceScaleAnimation_hpp

namespace RowBlast {
    class GameScene;
    
    class FallingPieceScaleAnimation {
    public:
        FallingPieceScaleAnimation(GameScene& scene);
        
        void Init();
        void StartScaleUp();
        void StartScaleDown();
        void Update(float dt);
        
    private:
        void UpdateInScalingUpState(float dt);
        void UpdateInScalingDownState(float dt);

        enum class State {
            ScalingUp,
            ScalingDown,
            Inactive
        };

        GameScene& mScene;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
    };
}

#endif
