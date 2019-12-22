#ifndef GhostPieceProducer_hpp
#define GhostPieceProducer_hpp

#include <memory>
#include <vector>

// Engine includes.
#include "SoftwareRasterizer.hpp"
#include "Vector.hpp"

// Game includes.
#include "Cell.hpp"

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
    
    struct GhostPieceBorderSegment {
        Pht::IVec2 mPosition;
        BorderSegmentKind mKind;
    };
    
    using GhostPieceBorder = std::vector<GhostPieceBorderSegment>;
    
    class GhostPieceProducer {
    public:
        struct GhostPieceRenderables {
            std::unique_ptr<Pht::RenderableObject> mDraggedPiece;
            std::unique_ptr<Pht::RenderableObject> mHighlightedDraggedPiece;
            std::unique_ptr<Pht::RenderableObject> mShadow;
            std::unique_ptr<Pht::RenderableObject> mGhostPiece;
            std::unique_ptr<Pht::RenderableObject> mHighlightedGhostPiece;
        };
        
        GhostPieceProducer(Pht::IEngine& engine,
                           const Pht::IVec2& pieceGridSize,
                           const CommonResources& commonResources);
        
        GhostPieceRenderables DrawRenderables(const GhostPieceBorder& border, BlockColor color);
        
    private:
        enum class GhostPieceKind {
            DraggedPiece,
            HighlightedDraggedPiece,
            Shadow,
            GhostPiece,
            HighlightedGhostPiece
        };
        
        std::unique_ptr<Pht::RenderableObject> DrawPiece(const GhostPieceBorder& border,
                                                         BlockColor color,
                                                         GhostPieceKind ghostPieceKind);
        std::unique_ptr<Pht::RenderableObject> ProduceRenderable() const;
        void Clear();
        void SetUpColors(BlockColor color, GhostPieceKind ghostPieceKind);
        void DrawBorder(const GhostPieceBorder& border);
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
        float mBorderOffset;
        float mBorderWidth;
        float mConnectionBorderWidth;
        Pht::Vec2 mCoordinateSystemSize;
        std::unique_ptr<Pht::SoftwareRasterizer> mRasterizer;
        Pht::Vec2 mSegmentStartPosition {0.0f, 0.0f};
        Pht::Vec4 mBorderColor;
        Pht::Vec4 mFillColor;
    };
}

#endif
