#include "GhostPieceProducer.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "IImage.hpp"
#include "RenderableObject.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    constexpr auto outerBorderWidth = 0.08f;
    constexpr auto innerBorderWidth = 0.08f;
    constexpr auto borderAlpha = 0.9f;
    constexpr auto fillAlpha = 0.22f;
    constexpr auto borderOffset = 0.0465f;

    const Pht::Vec4 redFillColor {1.0f, 0.5f, 0.5f, fillAlpha};
    const Pht::Vec4 greenFillColor {0.5f, 0.79f, 0.5f, fillAlpha};
    const Pht::Vec4 blueFillColor {0.3f, 0.72f, 1.0f, fillAlpha};
    const Pht::Vec4 yellowFillColor {0.9f, 0.85f, 0.0f, 0.205f};

    const Pht::Vec4 redBorderColor {1.0f, 0.705f, 0.745f, borderAlpha};
    const Pht::Vec4 greenBorderColor {0.6f, 0.905f, 0.6f, 0.94f};
    const Pht::Vec4 blueBorderColor {0.65f, 0.85f, 1.0f, borderAlpha};
    const Pht::Vec4 yellowBorderColor {0.95f, 0.85f, 0.0f, 0.875f};
    
    const Pht::Vec4 shadowColor {0.0f, 0.0f, 0.0f, 0.425f};
}

GhostPieceProducer::GhostPieceProducer(Pht::IEngine& engine,
                                       const Pht::IVec2& pieceGridSize,
                                       const CommonResources& commonResources) :
    mEngine {engine},
    mCellSize {commonResources.GetCellSize()},
    mCoordinateSystemSize {
        static_cast<float>(pieceGridSize.x) * mCellSize,
        static_cast<float>(pieceGridSize.y) * mCellSize
    } {
    
    auto& renderer = engine.GetRenderer();
    auto& renderBufferSize = renderer.GetRenderBufferSize();
    auto& frustumSize = commonResources.GetOrthographicFrustumSizePotentiallyZoomedScreen();
    
    auto xScaleFactor =
        mCellSize * static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x);
    
    auto yScaleFactor =
        mCellSize * static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y);
    
    Pht::IVec2 imageSize {
        static_cast<int>(static_cast<float>(pieceGridSize.x) * xScaleFactor) * 2,
        static_cast<int>(static_cast<float>(pieceGridSize.y) * yScaleFactor) * 2
    };
    
    mRasterizer = std::make_unique<Pht::SoftwareRasterizer>(mCoordinateSystemSize, imageSize);
}

void GhostPieceProducer::Clear() {
    mRasterizer->ClearBuffer();
}

GhostPieceProducer::GhostPieceRenderables
GhostPieceProducer::DrawRenderables(const GhostPieceBorder& border, BlockColor color) {
    return GhostPieceRenderables {
        .mDraggedPiece = DrawPiece(border, color, GhostPieceKind::DraggedPiece),
        .mHighlightedDraggedPiece = DrawPiece(border, color, GhostPieceKind::HighlightedDraggedPiece),
        .mShadow = DrawPiece(border, color, GhostPieceKind::Shadow),
        .mGhostPiece = DrawPiece(border, color, GhostPieceKind::GhostPiece),
        .mHighlightedGhostPiece = DrawPiece(border, color, GhostPieceKind::HighlightedGhostPiece)
    };
}

std::unique_ptr<Pht::RenderableObject>
GhostPieceProducer::DrawPiece(const GhostPieceBorder& border,
                              BlockColor color,
                              GhostPieceKind ghostPieceKind) {
    Clear();
    SetUpColors(color, ghostPieceKind);
    
    auto outerBorderAlpha = mBorderColor.w;
    auto alphaFactor = 2.5f;
    
    switch (ghostPieceKind) {
        case GhostPieceKind::DraggedPiece:
            mBorderColor = mBorderColor * 0.98f;
            break;
        case GhostPieceKind::HighlightedDraggedPiece:
            mBorderColor = mBorderColor * 1.1f;
            break;
        case GhostPieceKind::Shadow:
        case GhostPieceKind::GhostPiece:
            alphaFactor = 1.0f;
            break;
        case GhostPieceKind::HighlightedGhostPiece:
            break;
    }

    mBorderOffset = borderOffset;
    mBorderWidth = outerBorderWidth;
    mConnectionBorderWidth = mBorderWidth * 2.0f + innerBorderWidth;
    mBorderColor.w = std::min(outerBorderAlpha * alphaFactor, 1.0f);
    DrawBorder(border);
    
    mBorderOffset += mBorderWidth;
    mConnectionBorderWidth -= mBorderWidth * 2.0f;
    mBorderWidth = innerBorderWidth;
    mBorderColor.w = std::min(0.5f * outerBorderAlpha * alphaFactor, 1.0f);
    DrawBorder(border);

    if (ghostPieceKind != GhostPieceKind::Shadow) {
        mFillColor.w = std::min(mFillColor.w * alphaFactor, 1.0f);
        mRasterizer->FillEnclosedArea(mFillColor);
    }
    
    return ProduceRenderable();
}

std::unique_ptr<Pht::RenderableObject>
GhostPieceProducer::ProduceRenderable() const {
    auto image = mRasterizer->ProduceImage();
    
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    auto& sceneManager = mEngine.GetSceneManager();
    return sceneManager.CreateRenderableObject(Pht::QuadMesh {mCoordinateSystemSize.x,
                                                              mCoordinateSystemSize.y},
                                               imageMaterial);
}

void GhostPieceProducer::DrawBorder(const GhostPieceBorder& border) {
    for (auto& segment: border) {
        switch (segment.mKind) {
            case BorderSegmentKind::Start:
                break;
            case BorderSegmentKind::Upper:
                DrawUpperBorder(segment.mPosition);
                break;
            case BorderSegmentKind::Right:
            case BorderSegmentKind::RightForTriangle:
                DrawRightBorder(segment.mPosition, segment.mKind);
                break;
            case BorderSegmentKind::Lower:
            case BorderSegmentKind::LowerForPyramid:
            case BorderSegmentKind::LowerForTriangle:
                DrawLowerBorder(segment.mPosition, segment.mKind);
                break;
            case BorderSegmentKind::Left:
                DrawLeftBorder(segment.mPosition);
                break;
            case BorderSegmentKind::UpperLeftConcaveCorner:
                DrawUpperLeftConcaveCornerBorder(segment.mPosition);
                break;
            case BorderSegmentKind::LowerLeftConcaveCorner:
                DrawLowerLeftConcaveCornerBorder(segment.mPosition);
                break;
            case BorderSegmentKind::UpperRightConcaveCorner:
                DrawUpperRightConcaveCornerBorder(segment.mPosition);
                break;
            case BorderSegmentKind::LowerRightConcaveCorner:
                DrawLowerRightConcaveCornerBorder(segment.mPosition);
                break;
            case BorderSegmentKind::ConnectionForSeven:
                DrawConnectionForSeven(segment.mPosition);
                break;
            case BorderSegmentKind::ConnectionForMirroredSeven:
                DrawConnectionForMirroredSeven(segment.mPosition);
                break;
            case BorderSegmentKind::UpperLeftTiltForTriangle:
                DrawUpperLeftTiltedBorderForTriangle(segment.mPosition);
                break;
            case BorderSegmentKind::UpperLeftTiltForPyramid:
                DrawUpperLeftTiltedBorderForPyramid(segment.mPosition);
                break;
            case BorderSegmentKind::UpperRightTiltForPyramid:
                DrawUpperRightTiltedBorderForPyramid(segment.mPosition);
                break;
            case BorderSegmentKind::LowerRightTiltForDiamond:
                DrawLowerRightTiltedBorderForDiamond(segment.mPosition);
                break;
            case BorderSegmentKind::UpperRightTiltForDiamond:
                DrawUpperRightTiltedBorderForDiamond(segment.mPosition);
                break;
            case BorderSegmentKind::UpperLeftTiltForDiamond:
                DrawUpperLeftTiltedBorderForDiamond(segment.mPosition);
                break;
            case BorderSegmentKind::LowerLeftTiltForDiamond:
                DrawLowerLeftTiltedBorderForDiamond(segment.mPosition);
                break;
        }
        
        mSegmentStartPosition = Pht::Vec2 {
            static_cast<float>(segment.mPosition.x),
            static_cast<float>(segment.mPosition.y)
        };
    }
}

void GhostPieceProducer::SetUpColors(BlockColor color, GhostPieceKind ghostPieceKind) {
    if (ghostPieceKind == GhostPieceKind::Shadow) {
        mBorderColor = shadowColor;
    } else {
        switch (color) {
            case BlockColor::Red:
                mBorderColor = redBorderColor;
                mFillColor = redFillColor;
                break;
            case BlockColor::Green:
                mBorderColor = greenBorderColor;
                mFillColor = greenFillColor;
                break;
            case BlockColor::Blue:
                mBorderColor = blueBorderColor;
                mFillColor = blueFillColor;
                break;
            case BlockColor::Yellow:
                mBorderColor = yellowBorderColor;
                mFillColor = yellowFillColor;
                break;
            default:
                assert(false);
                break;
        }
    }
}

void GhostPieceProducer::DrawUpperBorder(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + mBorderOffset,
        segmentEndPosition.y * mCellSize - mBorderOffset - mBorderWidth
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize - mBorderOffset,
        mSegmentStartPosition.y * mCellSize - mBorderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight, lowerLeft, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawRightBorder(const Pht::IVec2& segmentEndPosition,
                                         BorderSegmentKind segmentKind) {
    auto extraOffset =
        segmentKind == BorderSegmentKind::RightForTriangle ? mBorderWidth + mBorderOffset : 0.0f;
    
    Pht::Vec2 lowerLeft {
        mSegmentStartPosition.x * mCellSize - mBorderOffset - mBorderWidth,
        mSegmentStartPosition.y * mCellSize + mBorderOffset
    };

    Pht::Vec2 upperRight {
        segmentEndPosition.x * mCellSize - mBorderOffset,
        segmentEndPosition.y * mCellSize - mBorderOffset - extraOffset
    };
    
    mRasterizer->DrawRectangle(upperRight, lowerLeft, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawLowerBorder(const Pht::IVec2& segmentEndPosition,
                                         BorderSegmentKind segmentKind) {
    auto extraOffsetLeft =
        (segmentKind == BorderSegmentKind::LowerForTriangle ||
         segmentKind == BorderSegmentKind::LowerForPyramid) ? mBorderWidth + mBorderOffset : 0.0f;

    auto extraOffsetRight =
        segmentKind == BorderSegmentKind::LowerForPyramid ? mBorderWidth + mBorderOffset : 0.0f;
    
    Pht::Vec2 lowerLeft {
        mSegmentStartPosition.x * mCellSize + mBorderOffset + extraOffsetLeft,
        mSegmentStartPosition.y * mCellSize + mBorderOffset
    };

    Pht::Vec2 upperRight {
        segmentEndPosition.x * mCellSize - mBorderOffset - extraOffsetRight,
        segmentEndPosition.y * mCellSize + mBorderOffset + mBorderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight, lowerLeft, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawLeftBorder(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + mBorderOffset,
        segmentEndPosition.y * mCellSize + mBorderOffset
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize + mBorderOffset + mBorderWidth,
        mSegmentStartPosition.y * mCellSize - mBorderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight, lowerLeft, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawUpperLeftConcaveCornerBorder(const Pht::IVec2& segmentPosition) {
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize + mBorderOffset,
        segmentPosition.y * mCellSize - mBorderOffset - mBorderWidth
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize + mBorderOffset + mBorderWidth,
        segmentPosition.y * mCellSize + mBorderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight1, lowerLeft1, mBorderColor);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize - mBorderOffset,
        segmentPosition.y * mCellSize - mBorderOffset - mBorderWidth
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize + mBorderOffset + mBorderWidth,
        segmentPosition.y * mCellSize - mBorderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight2, lowerLeft2, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawLowerLeftConcaveCornerBorder(const Pht::IVec2& segmentPosition) {
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize - mBorderOffset,
        segmentPosition.y * mCellSize + mBorderOffset
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize + mBorderOffset + mBorderWidth,
        segmentPosition.y * mCellSize + mBorderOffset + mBorderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight1, lowerLeft1, mBorderColor);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize + mBorderOffset,
        segmentPosition.y * mCellSize - mBorderOffset
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize + mBorderOffset + mBorderWidth,
        segmentPosition.y * mCellSize + mBorderOffset + mBorderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight2, lowerLeft2, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawUpperRightConcaveCornerBorder(const Pht::IVec2& segmentPosition) {
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize - mBorderOffset - mBorderWidth,
        segmentPosition.y * mCellSize - mBorderOffset - mBorderWidth
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize - mBorderOffset,
        segmentPosition.y * mCellSize + mBorderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight1, lowerLeft1, mBorderColor);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize - mBorderOffset - mBorderWidth,
        segmentPosition.y * mCellSize - mBorderOffset - mBorderWidth
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize + mBorderOffset,
        segmentPosition.y * mCellSize - mBorderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight2, lowerLeft2, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawLowerRightConcaveCornerBorder(const Pht::IVec2& segmentPosition) {
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize - mBorderOffset - mBorderWidth,
        segmentPosition.y * mCellSize + mBorderOffset
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize + mBorderOffset,
        segmentPosition.y * mCellSize + mBorderOffset + mBorderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight1, lowerLeft1, mBorderColor);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize - mBorderOffset - mBorderWidth,
        segmentPosition.y * mCellSize - mBorderOffset
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize - mBorderOffset,
        segmentPosition.y * mCellSize + mBorderOffset + mBorderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight2, lowerLeft2, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawConnectionForMirroredSeven(const Pht::IVec2& segmentPosition) {
    auto tiltedHalfmBorderWidth = std::sqrt(2.0f) * (mConnectionBorderWidth / 2.0f);
    
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize - mBorderOffset - tiltedHalfmBorderWidth,
        segmentPosition.y * mCellSize - mBorderOffset
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize + mBorderOffset,
        segmentPosition.y * mCellSize + mBorderOffset + tiltedHalfmBorderWidth
    };
    
    mRasterizer->DrawTiltedTrapezoid45(upperRight1, lowerLeft1, mConnectionBorderWidth / 2.0f, mBorderColor, Pht::DrawOver::Yes);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize - mBorderOffset,
        segmentPosition.y * mCellSize - mBorderOffset - tiltedHalfmBorderWidth
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize + mBorderOffset + tiltedHalfmBorderWidth,
        segmentPosition.y * mCellSize + mBorderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid315(upperRight2, lowerLeft2, mConnectionBorderWidth / 2.0f, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawConnectionForSeven(const Pht::IVec2& segmentPosition) {
    auto tiltedHalfmBorderWidth = std::sqrt(2.0f) * (mConnectionBorderWidth / 2.0f);
    
    Pht::Vec2 upperLeft1 {
        segmentPosition.x * mCellSize - mBorderOffset - tiltedHalfmBorderWidth,
        segmentPosition.y * mCellSize + mBorderOffset
    };

    Pht::Vec2 lowerRight1 {
        segmentPosition.x * mCellSize + mBorderOffset,
        segmentPosition.y * mCellSize + mBorderOffset + tiltedHalfmBorderWidth
    };
    
    mRasterizer->DrawTiltedTrapezoid225(upperLeft1, lowerRight1, mConnectionBorderWidth / 2.0f, mBorderColor, Pht::DrawOver::Yes);
    
    Pht::Vec2 upperLeft2 {
        segmentPosition.x * mCellSize - mBorderOffset,
        segmentPosition.y * mCellSize + mBorderOffset + tiltedHalfmBorderWidth
    };

    Pht::Vec2 lowerRight2 {
        segmentPosition.x * mCellSize + mBorderOffset + tiltedHalfmBorderWidth,
        segmentPosition.y * mCellSize - mBorderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid135(upperLeft2, lowerRight2, mConnectionBorderWidth / 2.0f, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawUpperLeftTiltedBorderForTriangle(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + mBorderOffset * 2.0f,
        segmentEndPosition.y * mCellSize + mBorderOffset
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize - mBorderOffset,
        mSegmentStartPosition.y * mCellSize - mBorderOffset * 2.0f
    };
    
    mRasterizer->DrawTiltedTrapezoid45(upperRight, lowerLeft, mBorderWidth, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawUpperLeftTiltedBorderForPyramid(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + mBorderOffset * 2.0f,
        segmentEndPosition.y * mCellSize + mBorderOffset
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize,
        mSegmentStartPosition.y * mCellSize - mBorderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid45(upperRight, lowerLeft, mBorderWidth, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawUpperRightTiltedBorderForPyramid(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 upperLeft {
        segmentEndPosition.x * mCellSize,
        segmentEndPosition.y * mCellSize - mBorderOffset
    };

    Pht::Vec2 lowerRight {
        mSegmentStartPosition.x * mCellSize - mBorderOffset * 2.0f,
        mSegmentStartPosition.y * mCellSize + mBorderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid135(upperLeft, lowerRight, mBorderWidth, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawLowerRightTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        mSegmentStartPosition.x * mCellSize,
        mSegmentStartPosition.y * mCellSize + mBorderOffset
    };

    Pht::Vec2 upperRight {
        segmentEndPosition.x * mCellSize - mBorderOffset,
        segmentEndPosition.y * mCellSize
    };
    
    mRasterizer->DrawTiltedTrapezoid315(upperRight, lowerLeft, mBorderWidth, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawUpperRightTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 upperLeft {
        segmentEndPosition.x * mCellSize,
        segmentEndPosition.y * mCellSize - mBorderOffset
    };

    Pht::Vec2 lowerRight {
        mSegmentStartPosition.x * mCellSize - mBorderOffset,
        mSegmentStartPosition.y * mCellSize
    };
    
    mRasterizer->DrawTiltedTrapezoid135(upperLeft, lowerRight, mBorderWidth, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawUpperLeftTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + mBorderOffset,
        segmentEndPosition.y * mCellSize
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize,
        mSegmentStartPosition.y * mCellSize - mBorderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid45(upperRight, lowerLeft, mBorderWidth, mBorderColor, Pht::DrawOver::Yes);
}

void GhostPieceProducer::DrawLowerLeftTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 upperLeft {
        mSegmentStartPosition.x * mCellSize + mBorderOffset,
        mSegmentStartPosition.y * mCellSize
    };

    Pht::Vec2 lowerRight {
        segmentEndPosition.x * mCellSize,
        segmentEndPosition.y * mCellSize + mBorderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid225(upperLeft, lowerRight, mBorderWidth, mBorderColor, Pht::DrawOver::Yes);
}
