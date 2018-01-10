#ifndef GameHud_hpp
#define GameHud_hpp

#include <memory>
#include <array>

// Engine includes.
#include "Vector.hpp"
#include "Font.hpp"
#include "SceneResources.hpp"

// Game includes.
#include "GradientRectangle.hpp"
#include "Level.hpp"
#include "IGameHudEventListener.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class GameLogic;
    class LevelResources;
    class Piece;
    class GameHudController;
    
    class GameHud: public IGameHudEventListener {
    public:
        GameHud(Pht::IEngine& engine,
                const GameLogic& gameLogic,
                const LevelResources& levelResources,
                GameHudController& gameHudController);
        
        void OnSwitchButtonDown() override;
        void OnSwitchButtonUp() override;
        
        void Reset(const Level& level);
        void Update();
        const Pht::RenderableObject* GetGrayBlock() const;
        const Pht::RenderableObject* GetBlueprintSlot() const;
        const GradientRectangle& GetSelectablePiecesRectangle() const;
        const GradientRectangle& GetSwitchTextRectangle() const;
        
        const Piece& GetLPiece() const {
            return mLPiece;
        }
        
        const Pht::Vec2& GetProgressPosition() const {
            return mProgressPosition;
        }
        
        const Pht::Vec2& GetMovesPosition() const {
            return mMovesPosition;
        }

        const Pht::Vec2& GetNextPiecesPosition() const {
            return mNextPiecesPosition;
        }

        const Pht::Vec2& GetSelectablePiecesPosition() const {
            return mSelectablePiecesPosition;
        }
    
        const Pht::Vec3& GetLightDirection() const {
            return mLightDirection;
        }

        using PiecePositions = std::array<Pht::Vec2, 2>;
        
        const PiecePositions& GetPieceRelativePositions() const {
            return mPieceRelativePositions;
        }

        const Pht::Text& GetProgressText() const {
            return mProgressText;
        }

        const Pht::Text& GetMovesText() const {
            return mMovesText;
        }

        const Pht::Text& GetNextText() const {
            return mNextText;
        }

        const Pht::Text& GetSwitchText() const {
            return mSwitchText;
        }

        const GradientRectangle& GetProgressTextRectangle() const{
            return *mProgressTextRectangle;
        }

        const GradientRectangle& GetMovesTextRectangle() const{
            return *mMovesTextRectangle;
        }

        const GradientRectangle& GetNextTextRectangle() const{
            return *mNextTextRectangle;
        }

        const GradientRectangle& GetProgressPiecesRectangle() const{
            return *mProgressPiecesRectangle;
        }

        const GradientRectangle& GetMovesPiecesRectangle() const{
            return *mMovesPiecesRectangle;
        }

        const GradientRectangle& GetNextPiecesRectangle() const{
            return *mNextPiecesRectangle;
        }
        
        float GetLPieceCellSize() const {
            return mLPieceCellSize;
        }
        
        const Pht::Vec2& GetLPieceRelativePosition() const {
            return mLPieceRelativePosition;
        }

        float GetGrayBlockSize() const {
            return mGrayBlockSize;
        }
        
        const Pht::Vec2& GetGrayBlockRelativePosition() const {
            return mGrayBlockRelativePosition;
        }

        float GetBlueprintSlotSize() const {
            return mBlueprintSlotSize;
        }
        
        const Pht::Vec2& GetBlueprintSlotRelativePosition() const {
            return mBlueprintSlotRelativePosition;
        }

    private:
        std::unique_ptr<GradientRectangle> CreatePiecesRectangle(const Pht::Vec3& position,
                                                                 bool isBright);
        std::unique_ptr<GradientRectangle> CreateSmallPiecesRectangle(const Pht::Vec3& position);
        std::unique_ptr<GradientRectangle> CreateTextRectangle(const Pht::Vec3& position,
                                                               float length,
                                                               bool isBright);
        void UpdateLightAnimation();
        void UpdateProgress();
        void UpdateMovesLeft();
        
        Pht::IEngine& mEngine;
        const GameLogic& mGameLogic;
        const Piece& mLPiece;
        const Pht::RenderableObject& mGrayBlock;
        const Pht::RenderableObject& mBlueprintSlot;
        Pht::SceneResources mSceneResources;
        Level::Objective mLevelObjective {Level::Objective::Clear};
        Pht::Vec2 mProgressPosition;
        Pht::Vec2 mMovesPosition;
        Pht::Vec2 mNextPiecesPosition;
        Pht::Vec2 mSelectablePiecesPosition;
        PiecePositions mPieceRelativePositions;
        Pht::Font mFont;
        Pht::Text mProgressText;
        Pht::Text mMovesText;
        Pht::Text mNextText;
        Pht::Text mSwitchText;
        std::unique_ptr<GradientRectangle> mProgressTextRectangle;
        std::unique_ptr<GradientRectangle> mMovesTextRectangle;
        std::unique_ptr<GradientRectangle> mNextTextRectangle;
        std::unique_ptr<GradientRectangle> mSwitchTextRectangle;
        std::unique_ptr<GradientRectangle> mSwitchTextRectangleBright;
        std::unique_ptr<GradientRectangle> mProgressPiecesRectangle;
        std::unique_ptr<GradientRectangle> mMovesPiecesRectangle;
        std::unique_ptr<GradientRectangle> mNextPiecesRectangle;
        std::unique_ptr<GradientRectangle> mSelectablePiecesRectangle;
        std::unique_ptr<GradientRectangle> mSelectablePiecesRectangleBright;
        int mMovesLeft {0};
        int mProgress {0};
        Pht::Vec3 mLightDirection;
        float mLightAnimationTime {0.0f};
        Pht::Vec2 mLPieceRelativePosition;
        float mLPieceCellSize {0.4f};
        Pht::Vec2 mGrayBlockRelativePosition;
        float mGrayBlockSize {0.63f};
        Pht::Vec2 mBlueprintSlotRelativePosition;
        float mBlueprintSlotSize {0.70f};
        bool mIsSwitchButtonDown {false};
    };
}

#endif
