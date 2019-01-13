#ifndef GhostPieceProducer_hpp
#define GhostPieceProducer_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "SoftwareRasterizer.hpp"
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class RenderableObject;
}

namespace RowBlast {
    class CommonResources;
    
    enum class BorderSegmentKind {
        Start,
        Upper,
        Right,
        RightForTriangle,
        Lower,
        LowerForTriangle,
        LowerForPyramid,
        Left,
        UpperLeftConcaveCorner,
        LowerLeftConcaveCorner,
        UpperRightConcaveCorner,
        LowerRightConcaveCorner,
        ConnectionForSeven,
        ConnectionForMirroredSeven,
        UpperLeftTiltForTriangle,
        UpperLeftTiltForPyramid,
        UpperRightTiltForPyramid,
        LowerRightTiltForDiamond,
        UpperRightTiltForDiamond,
        UpperLeftTiltForDiamond,
        LowerLeftTiltForDiamond
    };
    
    enum class FillGhostPiece {
        Yes,
        No
    };
    
    struct GhostPieceBorderSegment {
        Pht::IVec2 mPosition;
        BorderSegmentKind mKind;
    };
    
    using GhostPieceBorder = std::vector<GhostPieceBorderSegment>;
    
    class GhostPieceProducer {
    public:
        GhostPieceProducer(Pht::IEngine& engine,
                           float cellSize,
                           const Pht::IVec2& pieceGridSize,
                           const CommonResources& commonResources);
        
        void Clear();
        void DrawBorder(const GhostPieceBorder& border, FillGhostPiece fillGhostPiece);
        void SetBrightBorder();
        std::unique_ptr<Pht::RenderableObject> ProduceRenderable() const;
        
    private:
        void DrawUpperBorder(const Pht::IVec2& segmentEndPosition);
        void DrawRightBorder(const Pht::IVec2& segmentEndPosition, BorderSegmentKind segmentKind);
        void DrawLowerBorder(const Pht::IVec2& segmentEndPosition, BorderSegmentKind segmentKind);
        void DrawLeftBorder(const Pht::IVec2& segmentEndPosition);
        void DrawUpperLeftConcaveCornerBorder(const Pht::IVec2& segmentPosition);
        void DrawLowerLeftConcaveCornerBorder(const Pht::IVec2& segmentPosition);
        void DrawUpperRightConcaveCornerBorder(const Pht::IVec2& segmentPosition);
        void DrawLowerRightConcaveCornerBorder(const Pht::IVec2& segmentPosition);
        void DrawConnectionForSeven(const Pht::IVec2& segmentPosition);
        void DrawConnectionForMirroredSeven(const Pht::IVec2& segmentPosition);
        void DrawUpperLeftTiltedBorderForTriangle(const Pht::IVec2& segmentEndPosition);
        void DrawUpperLeftTiltedBorderForPyramid(const Pht::IVec2& segmentEndPosition);
        void DrawUpperRightTiltedBorderForPyramid(const Pht::IVec2& segmentEndPosition);
        void DrawLowerRightTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition);
        void DrawUpperRightTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition);
        void DrawUpperLeftTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition);
        void DrawLowerLeftTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition);

        Pht::IEngine& mEngine;
        float mCellSize;
        Pht::Vec2 mCoordinateSystemSize;
        std::unique_ptr<Pht::SoftwareRasterizer> mRasterizer;
        Pht::Vec2 mSegmentStartPosition {0.0f, 0.0f};
        Pht::Vec4 mBorderColor;
    };
}

#endif
