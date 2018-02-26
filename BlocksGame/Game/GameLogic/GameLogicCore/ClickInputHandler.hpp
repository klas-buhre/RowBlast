#ifndef ClickInputHandler_hpp
#define ClickInputHandler_hpp

// Engine include.
#include "SceneObject.hpp"
#include "Button.hpp"
#include "SwipeGestureRecognizer.hpp"

// Game includes.
#include "Ai.hpp"

namespace BlocksGame {
    class Field;
    class FallingPiece;
    class GameScene;
    class IGameLogic;
    class Level;
    
    class MoveButton {
    public:
        MoveButton(Pht::IEngine& engine);
        
        void SetPosition(const Pht::Vec3& position);
        void SetSize(const Pht::Vec2& size);
        
        Pht::Button& GetButton() {
            return mButton;
        }
        
    private:
        Pht::SceneObject mSceneObject;
        Pht::Button mButton;
    };
    
    class ClickInputHandler {
    public:
        static constexpr int maxNumVisibleMoves {20};
    
        using VisibleMoves = Pht::StaticVector<Move, ClickInputHandler::maxNumVisibleMoves>;
        
        ClickInputHandler(Pht::IEngine& engine,
                          Field& field,
                          const GameScene& gameScene,
                          IGameLogic& gameLogic);
        
        void Init(const Level& level);
        void CalculateMoves(const FallingPiece& fallingPiece);
        void UpdateMoves(const FallingPiece& fallingPiece);
        void CreateNewSetOfVisibleMoves();
        void HandleTouch(const Pht::TouchEvent& touchEvent);
        const VisibleMoves* GetVisibleMoves() const;
        
    private:
        void ClearClickGrid();
        void PopulateSetOfVisibleMoves();
        bool IsRoomForMove(const Move& move) const;
        void InsertMoveInClickGrid(const Move& move);
        void SetupButton(MoveButton& moveButton, Move& move);
        void HandleTouchBegin();
        void RecognizeSwipe(const Pht::TouchEvent& touchEvent);
        
        enum class State {
            PresentingMoves,
            Inactive
        };
        
        Pht::IEngine& mEngine;
        Field& mField;
        const GameScene& mGameScene;
        IGameLogic& mGameLogic;
        Ai mAi;
        State mState {State::Inactive};
        Ai::MovePtrs* mAllValidMoves {nullptr};
        VisibleMoves mVisibleMoves;
        std::vector<std::unique_ptr<MoveButton>> mMoveButtons;
        ClickGrid mClickGrid;
        int mNumClickGridRows {0};
        int mNumClickGridColumns {0};
        Pht::SwipeGestureRecognizer mSwipeUpRecognizer;
        bool mTouchContainsSwipeUp {false};
        const Piece* mPieceType {nullptr};
    };
}

#endif
