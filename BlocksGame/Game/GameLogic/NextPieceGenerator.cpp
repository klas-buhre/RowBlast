#include "NextPieceGenerator.hpp"

#include <assert.h>

using namespace BlocksGame;

void NextPieceGenerator::Reset(const std::vector<const Piece*>& pieceTypes) {
    mPieceTypes = &pieceTypes;
    
    GenerateSequence();
    
    mNext2Pieces[1] = GetNextFromSequence();
    mNext2Pieces[0] = GetNextFromSequence();
}

const Piece& NextPieceGenerator::GetNext() {
    auto* next {mNext2Pieces[1]};
    
    mNext2Pieces[1] = mNext2Pieces[0];
    mNext2Pieces[0] = GetNextFromSequence();
    
    return *next;
}

void NextPieceGenerator::GenerateSequence() {
    Pieces bag;
    
    for (auto* pieceType: *mPieceTypes) {
        bag.PushBack(pieceType);
    }
    
    mSequence.Clear();
    auto numPieceTypes {mPieceTypes->size()};
    
    for (auto i {0}; i < numPieceTypes; ++i) {
        auto pieceIndex {std::rand() % bag.Size()};
        mSequence.PushBack(bag.At(pieceIndex));
        bag.Erase(pieceIndex);
    }
    
    mIndexInSequence = 0;
}

const Piece* NextPieceGenerator::GetNextFromSequence() {
    if (mIndexInSequence >= mSequence.Size()) {
        GenerateSequence();
    }
    
    auto* next {mSequence.At(mIndexInSequence)};
    ++mIndexInSequence;
    
    return next;
}
