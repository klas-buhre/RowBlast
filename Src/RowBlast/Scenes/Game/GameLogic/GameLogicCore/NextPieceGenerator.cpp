#include "NextPieceGenerator.hpp"

#include <assert.h>

using namespace RowBlast;

void NextPieceGenerator::Init(const std::vector<const Piece*>& pieceTypes,
                              const std::vector<const Piece*>& initialPieceSequence) {
    mPieceTypes = &pieceTypes;
    
    GenerateSequence(initialPieceSequence);
    SetNext2Pieces();
}

void NextPieceGenerator::SetNext2Pieces() {
    mNext2Pieces[0] = GetNextFromSequence(Pht::Optional<int> {});
    mNext2Pieces[1] = GetNextFromSequence(Pht::Optional<int> {});
}

const Piece* NextPieceGenerator::GetNext(int numMovesLeft) {
    auto* next = mNext2Pieces[0];
    
    mNext2Pieces[0] = mNext2Pieces[1];
    mNext2Pieces[1] = GetNextFromSequence(numMovesLeft);
    
    return next;
}

void NextPieceGenerator::GenerateSequence(const std::vector<const Piece*>& initialPieceSequence) {
    Pieces bag;
    
    for (auto* pieceType: *mPieceTypes) {
        bag.PushBack(pieceType);
    }
    
    mSequence.Clear();
    
    for (auto* piece: initialPieceSequence) {
        mSequence.PushBack(piece);
        bag.EraseValue(piece);
    }
    
    auto numPieceTypes = bag.Size();
    
    for (auto i {0}; i < numPieceTypes; ++i) {
        auto pieceIndex = std::rand() % bag.Size();
        mSequence.PushBack(bag.At(pieceIndex));
        bag.Erase(pieceIndex);
    }
    
    mIndexInSequence = 0;
}

const Piece* NextPieceGenerator::GetNextFromSequence(Pht::Optional<int> numMovesLeft) {
    if (numMovesLeft.HasValue() && numMovesLeft.GetValue() < 5) {
        return nullptr;
    }
    
    if (mIndexInSequence >= mSequence.Size()) {
        GenerateSequence({});
    }
    
    auto* next = mSequence.At(mIndexInSequence);
    ++mIndexInSequence;
    
    return next;
}
