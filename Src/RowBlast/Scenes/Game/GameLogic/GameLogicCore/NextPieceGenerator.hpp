#ifndef NextPieceGenerator_hpp
#define NextPieceGenerator_hpp

#include <vector>
#include <array>

// Engine includes.
#include "StaticVector.hpp"

// Game includes.
#include "Piece.hpp"

namespace RowBlast {
    using TwoPieces = std::array<const Piece*, 2>;
    
    class NextPieceGenerator {
    public:
        void Init(const std::vector<const Piece*>& pieceTypes,
                  const std::vector<const Piece*>& initialPieceSequence);
        const Piece* GetNext(int numMovesLeft);
        void SetNext2Pieces();
        
        const TwoPieces& GetNext2Pieces() const {
            return mNext2Pieces;
        }
        
    private:
        void GenerateSequence(const std::vector<const Piece*>& initialPieceSequence);
        const Piece* GetNextFromSequence(Pht::Optional<int> numMovesLeft);
        
        using Pieces = Pht::StaticVector<const Piece*, static_cast<int>(Piece::numPieceTypes)>;
        
        const std::vector<const Piece*>* mPieceTypes {nullptr};
        Pieces mSequence;
        int mIndexInSequence {0};
        TwoPieces mNext2Pieces;
    };
}

#endif
