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
    const auto borderWidth {0.09f};
    const auto borderOffset {0.04f};
    const Pht::Vec4 borderColor {1.0f, 1.0f, 1.0f, 0.82f};
    const Pht::Vec4 brightBorderColor {1.0f, 1.0f, 1.0f, 1.0f};
    const Pht::Vec4 fillColor {1.0f, 1.0f, 1.0f, 0.42f};
}

GhostPieceProducer::GhostPieceProducer(Pht::IEngine& engine,
                                       const Pht::IVec2& pieceGridSize,
                                       const CommonResources& commonResources) :
    mEngine {engine},
    mCellSize {commonResources.GetCellSize()},
    mCoordinateSystemSize {
        static_cast<float>(pieceGridSize.x) * mCellSize,
        static_cast<float>(pieceGridSize.y) * mCellSize
    },
    mBorderColor {borderColor} {
    
    auto& renderer {engine.GetRenderer()};
    auto& renderBufferSize {renderer.GetRenderBufferSize()};
    auto& frustumSize {commonResources.GetOrthographicFrustumSizePotentiallyZoomedScreen()};
    
    auto xScaleFactor {
        mCellSize * static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)
    };
    
    auto yScaleFactor {
        mCellSize * static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)
    };
    
    Pht::IVec2 imageSize {
        static_cast<int>(static_cast<float>(pieceGridSize.x) * xScaleFactor) * 2,
        static_cast<int>(static_cast<float>(pieceGridSize.y) * yScaleFactor) * 2
    };
    
    mRasterizer = std::make_unique<Pht::SoftwareRasterizer>(mCoordinateSystemSize, imageSize);
}

void GhostPieceProducer::Clear() {
    mRasterizer->ClearBuffer();
}

void GhostPieceProducer::DrawBorder(const GhostPieceBorder& border, FillGhostPiece fillGhostPiece) {
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
    
    if (fillGhostPiece == FillGhostPiece::Yes) {
        mRasterizer->FillEnclosedArea(fillColor);
    }
}

void GhostPieceProducer::SetBrightBorder() {
    mBorderColor = brightBorderColor;
}

void GhostPieceProducer::DrawUpperBorder(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + borderOffset,
        segmentEndPosition.y * mCellSize - borderOffset - borderWidth
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize - borderOffset,
        mSegmentStartPosition.y * mCellSize - borderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight, lowerLeft, mBorderColor);
}

void GhostPieceProducer::DrawRightBorder(const Pht::IVec2& segmentEndPosition,
                                         BorderSegmentKind segmentKind) {
    auto extraOffset {
        segmentKind == BorderSegmentKind::RightForTriangle ? borderWidth + borderOffset : 0.0f
    };
    
    Pht::Vec2 lowerLeft {
        mSegmentStartPosition.x * mCellSize - borderOffset - borderWidth,
        mSegmentStartPosition.y * mCellSize + borderOffset
    };

    Pht::Vec2 upperRight {
        segmentEndPosition.x * mCellSize - borderOffset,
        segmentEndPosition.y * mCellSize - borderOffset - extraOffset
    };
    
    mRasterizer->DrawRectangle(upperRight, lowerLeft, mBorderColor);
}

void GhostPieceProducer::DrawLowerBorder(const Pht::IVec2& segmentEndPosition,
                                         BorderSegmentKind segmentKind) {
    auto extraOffsetLeft {
        (segmentKind == BorderSegmentKind::LowerForTriangle ||
         segmentKind == BorderSegmentKind::LowerForPyramid) ? borderWidth + borderOffset : 0.0f
    };

    auto extraOffsetRight {
        segmentKind == BorderSegmentKind::LowerForPyramid ? borderWidth + borderOffset : 0.0f
    };
    
    Pht::Vec2 lowerLeft {
        mSegmentStartPosition.x * mCellSize + borderOffset + extraOffsetLeft,
        mSegmentStartPosition.y * mCellSize + borderOffset
    };

    Pht::Vec2 upperRight {
        segmentEndPosition.x * mCellSize - borderOffset - extraOffsetRight,
        segmentEndPosition.y * mCellSize + borderOffset + borderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight, lowerLeft, mBorderColor);
}

void GhostPieceProducer::DrawLeftBorder(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + borderOffset,
        segmentEndPosition.y * mCellSize + borderOffset
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize + borderOffset + borderWidth,
        mSegmentStartPosition.y * mCellSize - borderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight, lowerLeft, mBorderColor);
}

void GhostPieceProducer::DrawUpperLeftConcaveCornerBorder(const Pht::IVec2& segmentPosition) {
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize + borderOffset,
        segmentPosition.y * mCellSize - borderOffset - borderWidth
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize + borderOffset + borderWidth,
        segmentPosition.y * mCellSize + borderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight1, lowerLeft1, mBorderColor);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize - borderOffset,
        segmentPosition.y * mCellSize - borderOffset - borderWidth
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize + borderOffset + borderWidth,
        segmentPosition.y * mCellSize - borderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight2, lowerLeft2, mBorderColor);
}

void GhostPieceProducer::DrawLowerLeftConcaveCornerBorder(const Pht::IVec2& segmentPosition) {
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize - borderOffset,
        segmentPosition.y * mCellSize + borderOffset
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize + borderOffset + borderWidth,
        segmentPosition.y * mCellSize + borderOffset + borderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight1, lowerLeft1, mBorderColor);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize + borderOffset,
        segmentPosition.y * mCellSize - borderOffset
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize + borderOffset + borderWidth,
        segmentPosition.y * mCellSize + borderOffset + borderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight2, lowerLeft2, mBorderColor);
}

void GhostPieceProducer::DrawUpperRightConcaveCornerBorder(const Pht::IVec2& segmentPosition) {
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize - borderOffset - borderWidth,
        segmentPosition.y * mCellSize - borderOffset - borderWidth
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize - borderOffset,
        segmentPosition.y * mCellSize + borderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight1, lowerLeft1, mBorderColor);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize - borderOffset - borderWidth,
        segmentPosition.y * mCellSize - borderOffset - borderWidth
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize + borderOffset,
        segmentPosition.y * mCellSize - borderOffset
    };
    
    mRasterizer->DrawRectangle(upperRight2, lowerLeft2, mBorderColor);
}

void GhostPieceProducer::DrawLowerRightConcaveCornerBorder(const Pht::IVec2& segmentPosition) {
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize - borderOffset - borderWidth,
        segmentPosition.y * mCellSize + borderOffset
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize + borderOffset,
        segmentPosition.y * mCellSize + borderOffset + borderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight1, lowerLeft1, mBorderColor);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize - borderOffset - borderWidth,
        segmentPosition.y * mCellSize - borderOffset
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize - borderOffset,
        segmentPosition.y * mCellSize + borderOffset + borderWidth
    };
    
    mRasterizer->DrawRectangle(upperRight2, lowerLeft2, mBorderColor);
}

void GhostPieceProducer::DrawConnectionForSeven(const Pht::IVec2& segmentPosition) {
    auto tiltedHalfBorderWidth {std::sqrt(2.0f) * (borderWidth / 2.0f)};
    
    Pht::Vec2 lowerLeft1 {
        segmentPosition.x * mCellSize - borderOffset - tiltedHalfBorderWidth,
        segmentPosition.y * mCellSize - borderOffset
    };

    Pht::Vec2 upperRight1 {
        segmentPosition.x * mCellSize + borderOffset,
        segmentPosition.y * mCellSize + borderOffset + tiltedHalfBorderWidth
    };
    
    mRasterizer->DrawTiltedTrapezoid45(upperRight1, lowerLeft1, borderWidth / 2.0f, mBorderColor);
    
    Pht::Vec2 lowerLeft2 {
        segmentPosition.x * mCellSize - borderOffset,
        segmentPosition.y * mCellSize - borderOffset - tiltedHalfBorderWidth
    };

    Pht::Vec2 upperRight2 {
        segmentPosition.x * mCellSize + borderOffset + tiltedHalfBorderWidth,
        segmentPosition.y * mCellSize + borderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid315(upperRight2, lowerLeft2, borderWidth / 2.0f, mBorderColor);
}

void GhostPieceProducer::DrawConnectionForMirroredSeven(const Pht::IVec2& segmentPosition) {
    auto tiltedHalfBorderWidth {std::sqrt(2.0f) * (borderWidth / 2.0f)};
    
    Pht::Vec2 upperLeft1 {
        segmentPosition.x * mCellSize - borderOffset - tiltedHalfBorderWidth,
        segmentPosition.y * mCellSize + borderOffset
    };

    Pht::Vec2 lowerRight1 {
        segmentPosition.x * mCellSize + borderOffset,
        segmentPosition.y * mCellSize + borderOffset + tiltedHalfBorderWidth
    };
    
    mRasterizer->DrawTiltedTrapezoid225(upperLeft1, lowerRight1, borderWidth / 2.0f, mBorderColor);
    
    Pht::Vec2 upperLeft2 {
        segmentPosition.x * mCellSize - borderOffset,
        segmentPosition.y * mCellSize + borderOffset + tiltedHalfBorderWidth
    };

    Pht::Vec2 lowerRight2 {
        segmentPosition.x * mCellSize + borderOffset + tiltedHalfBorderWidth,
        segmentPosition.y * mCellSize - borderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid135(upperLeft2, lowerRight2, borderWidth / 2.0f, mBorderColor);
}

void GhostPieceProducer::DrawUpperLeftTiltedBorderForTriangle(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + borderOffset * 2.0f,
        segmentEndPosition.y * mCellSize + borderOffset
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize - borderOffset,
        mSegmentStartPosition.y * mCellSize - borderOffset * 2.0f
    };
    
    mRasterizer->DrawTiltedTrapezoid45(upperRight, lowerLeft, borderWidth, mBorderColor);
}

void GhostPieceProducer::DrawUpperLeftTiltedBorderForPyramid(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + borderOffset * 2.0f,
        segmentEndPosition.y * mCellSize + borderOffset
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize,
        mSegmentStartPosition.y * mCellSize - borderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid45(upperRight, lowerLeft, borderWidth, mBorderColor);
}

void GhostPieceProducer::DrawUpperRightTiltedBorderForPyramid(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 upperLeft {
        segmentEndPosition.x * mCellSize,
        segmentEndPosition.y * mCellSize - borderOffset
    };

    Pht::Vec2 lowerRight {
        mSegmentStartPosition.x * mCellSize - borderOffset * 2.0f,
        mSegmentStartPosition.y * mCellSize + borderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid135(upperLeft, lowerRight, borderWidth, mBorderColor);
}

void GhostPieceProducer::DrawLowerRightTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        mSegmentStartPosition.x * mCellSize,
        mSegmentStartPosition.y * mCellSize + borderOffset
    };

    Pht::Vec2 upperRight {
        segmentEndPosition.x * mCellSize - borderOffset,
        segmentEndPosition.y * mCellSize
    };
    
    mRasterizer->DrawTiltedTrapezoid315(upperRight, lowerLeft, borderWidth, mBorderColor);
}

void GhostPieceProducer::DrawUpperRightTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 upperLeft {
        segmentEndPosition.x * mCellSize,
        segmentEndPosition.y * mCellSize - borderOffset
    };

    Pht::Vec2 lowerRight {
        mSegmentStartPosition.x * mCellSize - borderOffset,
        mSegmentStartPosition.y * mCellSize
    };
    
    mRasterizer->DrawTiltedTrapezoid135(upperLeft, lowerRight, borderWidth, mBorderColor);
}

void GhostPieceProducer::DrawUpperLeftTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 lowerLeft {
        segmentEndPosition.x * mCellSize + borderOffset,
        segmentEndPosition.y * mCellSize
    };

    Pht::Vec2 upperRight {
        mSegmentStartPosition.x * mCellSize,
        mSegmentStartPosition.y * mCellSize - borderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid45(upperRight, lowerLeft, borderWidth, mBorderColor);
}

void GhostPieceProducer::DrawLowerLeftTiltedBorderForDiamond(const Pht::IVec2& segmentEndPosition) {
    Pht::Vec2 upperLeft {
        mSegmentStartPosition.x * mCellSize + borderOffset,
        mSegmentStartPosition.y * mCellSize
    };

    Pht::Vec2 lowerRight {
        segmentEndPosition.x * mCellSize,
        segmentEndPosition.y * mCellSize + borderOffset
    };
    
    mRasterizer->DrawTiltedTrapezoid225(upperLeft, lowerRight, borderWidth, mBorderColor);
}

std::unique_ptr<Pht::RenderableObject> GhostPieceProducer::ProduceRenderable() const {
    auto image {mRasterizer->ProduceImage()};
    
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    auto& sceneManager {mEngine.GetSceneManager()};
    return sceneManager.CreateRenderableObject(Pht::QuadMesh {mCoordinateSystemSize.x,
                                                              mCoordinateSystemSize.y},
                                               imageMaterial);
}
