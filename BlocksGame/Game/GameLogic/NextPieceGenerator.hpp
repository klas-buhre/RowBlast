#ifndef NextPieceGenerator_hpp
#define NextPieceGenerator_hpp

#include <vector>
#include <array>

// Engine includes.
#include "StaticVector.hpp"

// Game includes.
#include "Piece.hpp"

namespace BlocksGame {
    using TwoPieces = std::array<const Piece*, 2>;
    
    class NextPieceGenerator {
    public:
        void Reset(const std::vector<const Piece*>& pieceTypes);
        const Piece& GetNext();
        
        const TwoPieces& GetNext2Pieces() const {
            return mNext2Pieces;
        }
        
    private:
        void GenerateSequence();
        const Piece* GetNextFromSequence();
        
        using Pieces = Pht::StaticVector<const Piece*, static_cast<int>(Piece::numPieceTypes)>;
        
        const std::vector<const Piece*>* mPieceTypes {nullptr};
        Pieces mSequence;
        int mIndexInSequence {0};
        TwoPieces mNext2Pieces;
    };
}

#endif
