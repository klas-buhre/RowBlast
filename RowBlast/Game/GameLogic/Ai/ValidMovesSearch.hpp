#ifndef ValidMovesSearch_hpp
#define ValidMovesSearch_hpp

#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "StaticVector.hpp"

// Game includes.
#include "Field.hpp"

namespace RowBlast {
    class Piece;
    
    class Movement {
    public:
        Movement() {}
        Movement(const Pht::Vec2& position, Rotation rotation, const Movement* previous);
        
        const Pht::Vec2& GetPosition() const {
            return mPosition;
        }
        
        Rotation GetRotation() const {
            return mRotation;
        }
        
        const Movement* GetPrevious() const {
            return mPrevious;
        }
        
    private:
        Pht::Vec2 mPosition;
        Rotation mRotation;
        const Movement* mPrevious {nullptr};
    };
    
    class MoveButton;
    
    struct Move {
        bool operator==(const Move& other) const {
            return mPosition == other.mPosition && mRotation == other.mRotation;
        }
        
        Pht::IVec2 mPosition;
        Rotation mRotation;
        const Movement* mLastMovement {nullptr};
        float mScore {0.0f};
        bool mHasBeenPresented {false};
        bool mIsReachable {true};
        MoveButton* mButton {nullptr};
    };
    
    using Moves = Pht::StaticVector<Move, Field::maxNumColumns * Field::maxNumRows * 4>;
    using Movements = Pht::StaticVector<Movement, Field::maxNumColumns * Field::maxNumRows * 4 * 2>;
    
    struct MovingPiece {
        void RotateClockwise();
        void RotateAntiClockwise();
        
        Pht::IVec2 mPosition;
        Rotation mRotation;
        const Piece& mPieceType;
    };
    
    struct ValidMoves {
        void Clear();
        
        Moves mMoves;
        Movements mMovements;
    };
    
    class ValidMovesSearch {
    public:
        explicit ValidMovesSearch(Field& field);
        
        void Init();
        void FindValidMoves(ValidMoves& validMoves, MovingPiece piece);
        
    private:
        enum class SearchDirection {
            Left,
            Right
        };
        
        enum class AllowRecursion {
            Yes,
            No
        };
        
        enum class PositionAdjustment {
            Yes,
            No
        };
        
        enum class SearchMovement {
            Start,
            Down,
            Right,
            Left,
            RotateClockwise,
            RotateAntiClockwise
        };
        
        enum class SearchCollisionResult {
            Collision,
            FoundMove,
            NoCollision
        };
        
        static constexpr int collisionNotCalculated {-1};
        
        struct SearchDataForOneRotation {
            Move* mFoundMove {nullptr};
            bool mIsVisited {false};
            int mCollisionColumnLeft {collisionNotCalculated};
            int mCollisionColumnRight {collisionNotCalculated};
            int mCollisionRow {collisionNotCalculated};
        };
 
        struct CellSearchData {
            SearchDataForOneRotation mData[4];
            const Movement* mFirstMovementAtLocation {nullptr};
            bool mUnderOverhangTip {false};
        };
        
        using SearchGrid = std::vector<std::vector<CellSearchData>>;

        void InitSearchGrid();
        void ResetVisitedLocations();
        void FindMostValidMovesWithHumanLikeSearch(ValidMoves& validMoves, MovingPiece piece);
        void AdjustPosition(MovingPiece& piece);
        bool CalculateFieldCellIsNotFilled(int row, int column) const;
        void FindValidMoves(ValidMoves& validMoves,
                            MovingPiece piece,
                            SearchDirection searchDirection,
                            const Movement* previousMovement,
                            AllowRecursion allowRecursion,
                            PositionAdjustment positionAdjustment);
        bool FindValidMovesForRotation(ValidMoves& validMoves,
                                       MovingPiece piece,
                                       SearchDirection searchDirection,
                                       const Movement* previousMovement,
                                       AllowRecursion allowRecursion,
                                       PositionAdjustment positionAdjustment);
        void SearchLeft(ValidMoves& validMoves,
                        MovingPiece piece,
                        const Movement* previousMovement,
                        AllowRecursion allowRecursion,
                        PositionAdjustment positionAdjustment);
        void SearchRight(ValidMoves& validMoves,
                         MovingPiece piece,
                         const Movement* previousMovement,
                         AllowRecursion allowRecursion,
                        PositionAdjustment positionAdjustment);
        void SearchDown(ValidMoves& validMoves,
                        MovingPiece piece,
                        const Movement* previousMovement,
                        AllowRecursion allowRecursion,
                        PositionAdjustment positionAdjustment);
        bool IsCollision(const MovingPiece& piece) const;
        void SaveMove(ValidMoves& validMoves,
                      const MovingPiece& piece,
                      const Movement* previousMovement);
        void FindAllRemainingValidMoves(ValidMoves& validMoves, MovingPiece piece);
        void Search(ValidMoves& validMoves,
                    MovingPiece piece,
                    const Movement* previousMovement,
                    SearchMovement searchMovement);
        void Search(MovingPiece piece, SearchMovement searchMovement);
        bool MovePieceAndCheckEdges(MovingPiece& piece, SearchMovement searchMovement);
        SearchCollisionResult HandleCollision(const MovingPiece& piece,
                                              SearchMovement searchMovement);
        void SaveMoveIfNotFoundBefore(ValidMoves& validMoves,
                                      const MovingPiece& piece,
                                      const Movement* previousMovement);
        bool IsDuplicateMoveFoundAtDifferentLocation(const MovingPiece& piece) const;
        const Movement* AddMovementAndRemoveDetour(ValidMoves& validMoves,
                                                   const MovingPiece& piece,
                                                   const Movement* previousMovement);
        bool IsLocationVisited(const MovingPiece& piece) const;
        void MarkLocationAsVisited(const MovingPiece& piece);
        SearchDataForOneRotation& GetSearchDataForOneRotation(const MovingPiece& piece);
        SearchDataForOneRotation& GetSearchDataForOneRotation(const Pht::IVec2& position,
                                                              Rotation rotation);
        const SearchDataForOneRotation& GetSearchDataForOneRotation(const Pht::IVec2& position,
                                                                    Rotation rotation) const;
        Pht::IVec2 CalculateSearchGridPosition(const Pht::IVec2& position) const;
        Move* GetFoundMove(const MovingPiece& piece) const;
        void SetFoundMove(const MovingPiece& piece, Move& move);
        int HandleCollisionLeft(const MovingPiece& piece);
        int HandleCollisionRight(const MovingPiece& piece);
        int HandleCollisionDown(const MovingPiece& piece);
        int DetectCollisionLeft(const MovingPiece& piece) const;
        int DetectCollisionRight(const MovingPiece& piece) const;
        int DetectCollisionDown(const MovingPiece& piece) const;
        
        Field& mField;
        SearchGrid mSearchGrid;
        mutable Field::CollisionResult mCollisionResult;
    };
}

#endif