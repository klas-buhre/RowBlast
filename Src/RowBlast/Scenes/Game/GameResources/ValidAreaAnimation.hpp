#ifndef ValidAreaAnimation_hpp
#define ValidAreaAnimation_hpp

#include <vector>

// Engine includes.
#include "StaticVector.hpp"

// Game includes.
#include "Field.hpp"
#include "MoveDefinitions.hpp"

namespace RowBlast {
    class Field;
    class Piece;
    class ScrollController;
    
    class ValidAreaAnimation {
    public:
        ValidAreaAnimation(Field& field, const ScrollController& scrollController);
        
        void Init();
        void Start(const Moves& allValidMoves, const Piece& pieceType, Rotation rotation);
        void Stop();
        bool IsActive() const;
        bool IsCellInvalid(int row, int column) const;
        
    private:
        struct InvalidMove {
            Pht::IVec2 mPosition;
            Rotation mRotation;
        };
        
        using InvalidMoves = Pht::StaticVector<InvalidMove, Field::maxNumColumns * Field::maxNumRows>;

        void FillValidAreaAboveMove(const Move& move, const Piece& pieceType);
        void FindInvalidMoves(InvalidMoves& invalidMoves,
                              const Piece& pieceType,
                              Rotation rotation);
        void ClearGrids();
        
        enum class State {
            Active,
            Inactive
        };
        
        enum class CellStatus {
            Undefined,
            Valid,
            Invalid
        };
        
        enum class ValidMoveBelow {
            Yes,
            No,
            Undefined
        };
        
        using Grid = std::vector<std::vector<CellStatus>>;
        using SearchData = std::vector<std::vector<ValidMoveBelow>>;
        
        Field& mField;
        const ScrollController& mScrollController;
        State mState;
        Grid mGrid;
        SearchData mSearchData;
    };
}

#endif
