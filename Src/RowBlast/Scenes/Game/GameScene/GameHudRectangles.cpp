#include "GameHudRectangles.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ISceneManager.hpp"
#include "SoftwareRasterizer.hpp"
#include "IImage.hpp"
#include "QuadMesh.hpp"

// Game includes.
#include "CommonResources.hpp"

using namespace RowBlast;

namespace {
    const Pht::Vec4 stencilColor {1.0f, 1.0f, 1.0f, 1.0f};
    const Pht::Vec4 outerBorderColor {0.6f, 0.3f, 0.75f, 0.7f};
    const Pht::Vec4 lowerOuterBorderColor {0.6f, 0.3f, 0.75f, 0.1f};
    const Pht::Vec4 borderColor {0.725f, 0.35f, 0.85f, 0.75f};
    const Pht::Vec4 lowerBorderColor {0.725f, 0.35f, 0.85f, 0.1f};
    const Pht::Vec4 innerColor {0.85f, 0.4f, 0.95f, 0.93f};
    const Pht::Vec4 lowerInnerColor {0.9f, 0.9f, 1.0f, 0.0f};
    const Pht::Vec4 pressedColorSubtract {0.1f, 0.1f, 0.1f, 0.0f};

    const Pht::Vec2 pauseButtonRectangleSize {1.5f, 1.5f};
    const Pht::Vec2 nextPiecesRectangleSize {4.5f, 2.4f};
    const Pht::Vec2 selectablePiecesRectangleSize {6.8f, 2.4f};
    constexpr auto dividerLineXPosition {4.35f};
    constexpr auto dividerLineThickness {0.02f};
    constexpr auto borderThickness {0.055f};
    constexpr auto piecesRectangleOuterCornerRadius {0.25f};
    constexpr auto piecesRectangleTilt {0.6f};
    constexpr auto pauseButtonRectangleTilt {0.375f};

    std::unique_ptr<Pht::SoftwareRasterizer> CreateRasterizer(Pht::IEngine& engine,
                                                              const CommonResources& commonResources,
                                                              const Pht::Vec2& size) {
        auto& renderer {engine.GetRenderer()};
        auto& renderBufferSize {renderer.GetRenderBufferSize()};
        auto& frustumSize {commonResources.GetHudFrustumSizePotentiallyZoomedScreen()};
        
        auto xScaleFactor {
            static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)
        };

        auto yScaleFactor {
            static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)
        };
        
        Pht::IVec2 imageSize {
            static_cast<int>(size.x * xScaleFactor),
            static_cast<int>(size.y * yScaleFactor)
        };
    
        return std::make_unique<Pht::SoftwareRasterizer>(size, imageSize);
    }
    
    Pht::Vec4 PositiveSubtract(const Pht::Vec4& a, const Pht::Vec4& b) {
        return {
            std::fmax(a.x - b.x, 0.0f),
            std::fmax(a.y - b.y, 0.0f),
            std::fmax(a.z - b.z, 0.0f),
            std::fmax(a.w - b.w, 0.0f)
        };
    }
}

GameHudRectangles::GameHudRectangles(Pht::IEngine& engine, const CommonResources& commonResources) {
    mPauseButtonRectangle = CreateRectangle(engine,
                                            commonResources,
                                            pauseButtonRectangleSize,
                                            pauseButtonRectangleTilt,
                                            {0.0f, 0.0f, 0.0f, 0.0f},
                                            false);
    mPressedPauseButtonRectangle = CreateRectangle(engine,
                                                   commonResources,
                                                   pauseButtonRectangleSize,
                                                   pauseButtonRectangleTilt,
                                                   pressedColorSubtract,
                                                   false);
    mNextPiecesRectangle = CreateRectangle(engine,
                                           commonResources,
                                           nextPiecesRectangleSize,
                                           piecesRectangleTilt,
                                           {0.0f, 0.0f, 0.0f, 0.0f},
                                           false);
    mSelectablePiecesRectangle = CreateRectangle(engine,
                                                 commonResources,
                                                 selectablePiecesRectangleSize,
                                                 piecesRectangleTilt,
                                                 {0.0f, 0.0f, 0.0f, 0.0f},
                                                 true);
    mPressedSelectablePiecesRectangle = CreateRectangle(engine,
                                                        commonResources,
                                                        selectablePiecesRectangleSize,
                                                        piecesRectangleTilt,
                                                        pressedColorSubtract,
                                                        true);
}

std::unique_ptr<Pht::RenderableObject>
GameHudRectangles::CreateRectangle(Pht::IEngine& engine,
                                   const CommonResources& commonResources,
                                   const Pht::Vec2& size,
                                   float tilt,
                                   const Pht::Vec4& colorSubtract,
                                   bool drawDividerLine) {
    auto rasterizer {CreateRasterizer(engine, commonResources, size)};

    DrawRectangleBorder(*rasterizer, size, colorSubtract);
    FillStencilBuffer(*rasterizer,
                      size,
                      piecesRectangleOuterCornerRadius - borderThickness * 2.0f,
                      borderThickness * 2.0f);
    DrawRectangleMainArea(*rasterizer, size, colorSubtract);
    
    if (drawDividerLine) {
        DrawLine(*rasterizer, colorSubtract);
    }

    auto image {rasterizer->ProduceImage()};
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    Pht::QuadMesh quadMesh {size.x, size.y, tilt};
    return engine.GetSceneManager().CreateRenderableObject(quadMesh, imageMaterial);
}

void GameHudRectangles::FillStencilBuffer(Pht::SoftwareRasterizer& rasterizer,
                                          const Pht::Vec2& size,
                                          float cornerRadius,
                                          float padding) {
    rasterizer.SetStencilBufferFillMode();
    
    rasterizer.DrawCircle({cornerRadius + padding, cornerRadius + padding},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    rasterizer.DrawCircle({size.x - cornerRadius - padding, cornerRadius + padding},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    rasterizer.DrawCircle({size.x - cornerRadius - padding, size.y - cornerRadius - padding},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    rasterizer.DrawCircle({cornerRadius + padding, size.y - cornerRadius - padding},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    
    Pht::Vec2 lowerLeft1 {cornerRadius + padding, padding};
    Pht::Vec2 upperRight1 {size.x - cornerRadius - padding, size.y - padding};
    rasterizer.DrawRectangle(upperRight1, lowerLeft1, stencilColor);

    Pht::Vec2 lowerLeft2 {padding, cornerRadius + padding};
    Pht::Vec2 upperRight2 {cornerRadius + padding, size.y - cornerRadius - padding};
    rasterizer.DrawRectangle(upperRight2, lowerLeft2, stencilColor);

    Pht::Vec2 lowerLeft3 {size.x - cornerRadius - padding, cornerRadius + padding};
    Pht::Vec2 upperRight3 {size.x - padding, size.y - cornerRadius - padding};
    rasterizer.DrawRectangle(upperRight3, lowerLeft3, stencilColor);

    rasterizer.EnableStencilTest();
}

void GameHudRectangles::DrawRectangleBorder(Pht::SoftwareRasterizer& rasterizer,
                                            const Pht::Vec2& size,
                                            const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer, size, piecesRectangleOuterCornerRadius, 0.0f);
    
    Pht::SoftwareRasterizer::VerticalGradientColors outerBorderColors {
        PositiveSubtract(lowerOuterBorderColor, colorSubtract),
        PositiveSubtract(outerBorderColor, colorSubtract)
    };
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRight, lowerLeft, outerBorderColors, Pht::DrawOver::Yes);

    FillStencilBuffer(rasterizer,
                      size,
                      piecesRectangleOuterCornerRadius - borderThickness,
                      borderThickness);

    Pht::SoftwareRasterizer::VerticalGradientColors borderColors {
        PositiveSubtract(lowerBorderColor, colorSubtract),
        PositiveSubtract(borderColor, colorSubtract)
    };
    Pht::Vec2 lowerLeft2 {0.0f, 0.0f};
    Pht::Vec2 upperRight2 {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRight2, lowerLeft2, borderColors, Pht::DrawOver::Yes);
}

void GameHudRectangles::DrawRectangleMainArea(Pht::SoftwareRasterizer& rasterizer,
                                              const Pht::Vec2& size,
                                              const Pht::Vec4& colorSubtract) {
    Pht::SoftwareRasterizer::VerticalGradientColors upperRectangleColors {
        PositiveSubtract(lowerInnerColor, colorSubtract),
        PositiveSubtract(innerColor, colorSubtract)
    };
    Pht::Vec2 lowerLeft2 {0.0f, 0.0f};
    Pht::Vec2 upperRight2 {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRight2,
                                     lowerLeft2,
                                     upperRectangleColors,
                                     Pht::DrawOver::Yes);
}

void GameHudRectangles::DrawLine(Pht::SoftwareRasterizer& rasterizer,
                                 const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer,
                      selectablePiecesRectangleSize,
                      piecesRectangleOuterCornerRadius,
                      0.0f);

    Pht::SoftwareRasterizer::VerticalGradientColors outerLineColors {
        PositiveSubtract(lowerBorderColor, colorSubtract),
        PositiveSubtract(borderColor, colorSubtract)
    };
    Pht::Vec2 lowerLeft1 {dividerLineXPosition, borderThickness};
    Pht::Vec2 upperRight1 {
        dividerLineXPosition + dividerLineThickness * 3.0f,
        selectablePiecesRectangleSize.y - borderThickness};
    rasterizer.DrawGradientRectangle(upperRight1, lowerLeft1, outerLineColors, Pht::DrawOver::Yes);
    
    Pht::SoftwareRasterizer::VerticalGradientColors innerLineColors {
        PositiveSubtract(lowerOuterBorderColor, colorSubtract),
        PositiveSubtract(outerBorderColor, colorSubtract)
    };

    Pht::Vec2 lowerLeft2 {dividerLineXPosition + dividerLineThickness, 0.0f};
    Pht::Vec2 upperRight2 {
        dividerLineXPosition + dividerLineThickness * 2.0f,
        selectablePiecesRectangleSize.y
    };
    rasterizer.DrawGradientRectangle(upperRight2, lowerLeft2, innerLineColors, Pht::DrawOver::Yes);
}
