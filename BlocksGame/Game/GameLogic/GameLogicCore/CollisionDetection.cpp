#include "CollisionDetection.hpp"

// Game includes.
#include "Piece.hpp"

using namespace RowBlast;

namespace {
    Pht::Vec2 CalculateCenter(const Field::CollisionPoints& collisionPoints) {
        Pht::Vec2 center {0.0f, 0.0f};
        
        for (auto i {0}; i < collisionPoints.Size(); ++i) {
            auto& iVec {collisionPoints.At(i)};
            Pht::Vec2 vec {iVec.x + 0.5f, iVec.y + 0.5f};
            center += vec;
        }
        
        return center / collisionPoints.Size();
    }

    Direction VectorToDirection(const Pht::Vec2& vec) {
        if (std::fabs(vec.x) >= std::fabs(vec.y)) {
            if (vec.x > 0) {
                return Direction::Right;
            } else {
                return Direction::Left;
            }
        } else {
            if (vec.y > 0) {
                return Direction::Up;
            } else {
                return Direction::Down;
            }
        }
    }
    
    Intersection HalfSubCellsIntersectNoScan(const SubCell& field, const SubCell& piece) {
        switch (field.mFill) {
            case Fill::LowerRightHalf:
                return piece.mFill == Fill::UpperLeftHalf ? Intersection::No : Intersection::Yes;
            case Fill::UpperRightHalf:
                return piece.mFill == Fill::LowerLeftHalf ? Intersection::No : Intersection::Yes;
            case Fill::UpperLeftHalf:
                return piece.mFill == Fill::LowerRightHalf ? Intersection::No : Intersection::Yes;
            case Fill::LowerLeftHalf:
                return piece.mFill == Fill::UpperRightHalf ? Intersection::No : Intersection::Yes;
            default:
                return Intersection::No;
        }
    }
    
    Intersection HalfSubCellsIntersectScanDown(const SubCell& field,
                                               const SubCell& piece,
                                               bool isScanStart) {
        switch (field.mFill) {
            case Fill::LowerRightHalf:
                return piece.mFill == Fill::UpperLeftHalf ? Intersection::NextWillBe : Intersection::Yes;
            case Fill::UpperRightHalf:
                if (piece.mFill == Fill::LowerLeftHalf && isScanStart) {
                    return Intersection::No;
                }
                return Intersection::Yes;
            case Fill::UpperLeftHalf:
                if (piece.mFill == Fill::LowerRightHalf && isScanStart) {
                    return Intersection::No;
                }
                return Intersection::Yes;
            case Fill::LowerLeftHalf:
                return piece.mFill == Fill::UpperRightHalf ? Intersection::NextWillBe : Intersection::Yes;
            default:
                return Intersection::No;
        }
    }

    Intersection HalfSubCellsIntersectScanLeft(const SubCell& field,
                                               const SubCell& piece,
                                               bool isScanStart) {
        switch (field.mFill) {
            case Fill::LowerRightHalf:
                if (piece.mFill == Fill::UpperLeftHalf && isScanStart) {
                    return Intersection::No;
                }
                return Intersection::Yes;
            case Fill::UpperRightHalf:
                if (piece.mFill == Fill::LowerLeftHalf && isScanStart) {
                    return Intersection::No;
                }
                return Intersection::Yes;
            case Fill::UpperLeftHalf:
                return piece.mFill == Fill::LowerRightHalf ? Intersection::NextWillBe : Intersection::Yes;
            case Fill::LowerLeftHalf:
                return piece.mFill == Fill::UpperRightHalf ? Intersection::NextWillBe : Intersection::Yes;
            default:
                return Intersection::No;
        }
    }

    Intersection HalfSubCellsIntersectScanRight(const SubCell& field,
                                                const SubCell& piece,
                                                bool isScanStart) {
        switch (field.mFill) {
            case Fill::LowerRightHalf:
                return piece.mFill == Fill::UpperLeftHalf ? Intersection::NextWillBe : Intersection::Yes;
            case Fill::UpperRightHalf:
                return piece.mFill == Fill::LowerLeftHalf ? Intersection::NextWillBe : Intersection::Yes;
            case Fill::UpperLeftHalf:
                if (piece.mFill == Fill::LowerRightHalf && isScanStart) {
                    return Intersection::No;
                }
                return Intersection::Yes;
            case Fill::LowerLeftHalf:
                if (piece.mFill == Fill::UpperRightHalf && isScanStart) {
                    return Intersection::No;
                }
                return Intersection::Yes;
            default:
                return Intersection::No;
        }
    }
}

Direction
CollisionDetection::CalculateCollisionDirection(const Field::CollisionPoints& collisionPoints,
                                                const PieceBlocks& pieceBlocks,
                                                Rotation pieceRotation,
                                                const Pht::IVec2& position,
                                                const Field& field) {
    auto collisionCenter {CalculateCenter(collisionPoints)};
    
    Pht::Vec2 pieceCenter {
        pieceBlocks.mNumColumns / 2.0f,
        pieceBlocks.mNumRows / 2.0f
    };
    
    auto directionFromCenter {collisionCenter - pieceCenter};
    
    if (std::fabs(directionFromCenter.x) == std::fabs(directionFromCenter.y)) {
        auto occupiedPoints {field.GetOccupiedArea(pieceBlocks, position)};
        auto occupiedPointsDirectionFromCenter {CalculateCenter(occupiedPoints) - pieceCenter};
        return VectorToDirection(occupiedPointsDirectionFromCenter);
    }
    
    return VectorToDirection(directionFromCenter);
}

Intersection CollisionDetection::SubCellsIntersect(const SubCell& field,
                                                   const SubCell& piece,
                                                   const Pht::IVec2& scanDirection,
                                                   bool isScanStart) {
    if (field.IsEmpty() || piece.IsEmpty()) {
        return Intersection::No;
    }
    
    if (field.IsFull() || piece.IsFull()) {
        return Intersection::Yes;
    }
    
    if (scanDirection == Pht::IVec2 {0, -1}) {
        return HalfSubCellsIntersectScanDown(field, piece, isScanStart);
    } else if (scanDirection == Pht::IVec2 {-1, 0}) {
        return HalfSubCellsIntersectScanLeft(field, piece, isScanStart);
    } else if (scanDirection == Pht::IVec2 {1, 0}) {
        return HalfSubCellsIntersectScanRight(field, piece, isScanStart);
    } else {
        return HalfSubCellsIntersectNoScan(field, piece);
    }
}
