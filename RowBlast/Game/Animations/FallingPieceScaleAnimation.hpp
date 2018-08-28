#ifndef FallingPieceScaleAnimation_hpp
#define FallingPieceScaleAnimation_hpp

namespace RowBlast {
    class GameScene;
    
    class FallingPieceScaleAnimation {
    public:
        FallingPieceScaleAnimation(GameScene& scene);
        
        void Init();
        void Start();
        void Update(float dt);
        
    private:
        void UpdateInActiveState(float dt);

        enum class State {
            Active,
            Inactive
        };

        GameScene& mScene;
        State mState {State::Inactive};
        float mElapsedTime {0.0f};
    };
}

#endif
