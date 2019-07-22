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
    constexpr auto borderWidth = 0.085f;
    constexpr auto borderAlpha = 0.825f;
    constexpr auto borderOffset = 0.04f;
    constexpr auto pressedPieceBorderAlpha = 1.0f;
    constexpr auto pressedPieceFillAlpha = 0.6f;
    constexpr auto fillAlpha = 0.19f;
    
    const Pht::Vec4 redFillColor {1.0f, 0.5f, 0.5f, fillAlpha};
    const Pht::Vec4 greenFillColor {0.5f, 0.79f, 0.5f, fillAlpha};
    const Pht::Vec4 blueFillColor {0.3f, 0.72f, 1.0f, fillAlpha};
    const Pht::Vec4 yellowFillColor {0.875f, 0.75f, 0.0f, fillAlpha};

    const Pht::Vec4 redBorderColor {1.0f, 0.8f, 0.8f, borderAlpha};
    const Pht::Vec4 greenBorderColor {0.6f, 0.85f, 0.6f, borderAlpha};
    const Pht::Vec4 blueBorderColor {0.65f, 0.85f, 1.0f, borderAlpha};
    const Pht::Vec4 yellowBorderColor {0.93f, 0.78f, 0.4f, borderAlpha};
    
    const Pht::Vec4 shadowColor {0.14f, 0.14f, 0.14f, 0.55f};
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

void GhostPieceProducer::DrawBorder(const GhostPieceBorder& border,
                                    BlockColor color,
                                    PressedGhostPiece pressedGhostPiece) {
    SetUpColors(color, pressedGhostPiece);

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
    
    mRasterizer->FillEnclosedArea(mFillColor);
}

void GhostPieceProducer::SetUpColors(BlockColor color, PressedGhostPiece pressedGhostPiece) {
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
    
    if (pressedGhostPiece == PressedGhostPiece::Yes) {
        mBorderColor.w = pressedPieceBorderAlpha;
        mFillColor.w = pressedPieceFillAlpha;
    }
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
    auto extraOffset =
        segmentKind == BorderSegmentKind::RightForTriangle ? borderWidth + borderOffset : 0.0f;
    
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
    auto extraOffsetLeft =
        (segmentKind == BorderSegmentKind::LowerForTriangle ||
         segmentKind == BorderSegmentKind::LowerForPyramid) ? borderWidth + borderOffset : 0.0f;

    auto extraOffsetRight =
        segmentKind == BorderSegmentKind::LowerForPyramid ? borderWidth + borderOffset : 0.0f;
    
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
    auto tiltedHalfBorderWidth = std::sqrt(2.0f) * (borderWidth / 2.0f);
    
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
    auto tiltedHalfBorderWidth = std::sqrt(2.0f) * (borderWidth / 2.0f);
    
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

std::unique_ptr<Pht::RenderableObject> GhostPieceProducer::ProducePressedRenderable() const {
    auto image = mRasterizer->ProduceImage();
    
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    auto& sceneManager = mEngine.GetSceneManager();
    return sceneManager.CreateRenderableObject(Pht::QuadMesh {mCoordinateSystemSize.x,
                                                              mCoordinateSystemSize.y},
                                               imageMaterial);
}

GhostPieceProducer::GhostPieceRenderables
GhostPieceProducer::ProduceRenderables(const std::string& pieceName) const {
    GhostPieceRenderables renderables;
    auto& sceneManager = mEngine.GetSceneManager();
    auto image = mRasterizer->ProduceImage();

    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes, pieceName};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    renderables.mRenderable =
        sceneManager.CreateRenderableObject(Pht::QuadMesh {mCoordinateSystemSize.x,
                                                           mCoordinateSystemSize.y},
                                            imageMaterial);

    Pht::Material shadowImageMaterial {*image, Pht::GenerateMipmap::Yes, pieceName};
    shadowImageMaterial.SetShaderId(Pht::ShaderId::TexturedLighting);
    shadowImageMaterial.SetBlend(Pht::Blend::Yes);
    shadowImageMaterial.SetOpacity(shadowColor.w);
    shadowImageMaterial.SetAmbient(Pht::Color{shadowColor.x, shadowColor.y, shadowColor.z});

    renderables.mShadowRenderable =
        sceneManager.CreateRenderableObject(Pht::QuadMesh {mCoordinateSystemSize.x,
                                                           mCoordinateSystemSize.y},
                                            shadowImageMaterial);

    return renderables;
}
