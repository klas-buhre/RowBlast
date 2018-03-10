#ifndef PreviewPiecesAnimation_hpp
#define PreviewPiecesAnimation_hpp

namespace BlocksGame {
    class GameScene;
    class GameLogic;
    
    class PreviewPiecesAnimation {
    public:
        PreviewPiecesAnimation(GameScene& scene, GameLogic& gameLogic);
        
        void Update(float dt);
        
    private:
        GameScene& mScene;
        GameLogic& mGameLogic;
    };
}

#endif
