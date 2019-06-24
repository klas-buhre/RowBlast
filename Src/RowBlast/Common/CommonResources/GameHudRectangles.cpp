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
#include "GameHud.hpp"

using namespace RowBlast;

namespace {
    constexpr auto rbAdd = 0.025f;
    const Pht::Vec4 stencilColor {1.0f, 1.0f, 1.0f, 1.0f};
    const Pht::Vec4 outerBorderColor {0.4f + rbAdd, 0.3f, 0.55f + rbAdd, 0.7f};
    const Pht::Vec4 lowerOuterBorderColor {0.4f + rbAdd, 0.3f, 0.55f + rbAdd, 0.1f};
    const Pht::Vec4 borderColor {0.525f + rbAdd, 0.35f, 0.65f + rbAdd, 0.75f};
    const Pht::Vec4 lowerBorderColor {0.525f + rbAdd, 0.35f, 0.65f + rbAdd, 0.1f};
    const Pht::Vec4 innerColor {0.65f + rbAdd, 0.4f, 0.75f + rbAdd, 0.93f};
    const Pht::Vec4 lowerInnerColor {0.9f, 0.9f, 1.0f, 0.0f};
    const Pht::Vec4 pressedColorSubtract {0.1f, 0.1f, 0.1f, 0.0f};
    
    
    constexpr auto f = 0.9f;
    // constexpr auto lowerAlpha = 0.1f;
    constexpr auto lowerAlpha = 0.25f;
    const Pht::Vec4 activeSlotInnerColor {0.53f * f, 0.2625f * f, 0.6085f * f, 0.96f};
    const Pht::Vec4 activeSlotLowerInnerColor {activeSlotInnerColor.ToVec3(), activeSlotInnerColor.w * lowerAlpha};

    const Pht::Vec4 activeSlotOuterBorderColor {0.7f, 0.525f, 0.825f, 0.85f};
    const Pht::Vec4 activeSlotOuterLowerBorderColor {activeSlotOuterBorderColor.ToVec3(), activeSlotOuterBorderColor.w * lowerAlpha};
    const auto activeSlotInnerBorderColorAdd = Pht::Vec4{activeSlotOuterBorderColor.ToVec3(), 0.0f} * 0.175f;
    const auto activeSlotInnerBorderColor = activeSlotInnerColor + activeSlotInnerBorderColorAdd;
    const auto activeSlotInnerLowerBorderColor = activeSlotLowerInnerColor + activeSlotInnerBorderColorAdd;
    
    constexpr auto  gridLineWidth = 0.05f;
    const Pht::Vec4 gridLineColor {1.0f, 1.0f, 1.0f, 0.06f};

    const Pht::Vec2 pauseButtonRectangleSize {1.5f, 1.5f};
    const auto nextPiecesRectangleSize = Pht::Vec2{4.5f, 2.4f} * GameHud::nextPiecesScale;
    constexpr auto nextPiecesRectangleTilt = 0.6f * GameHud::nextPiecesScale;
    const auto selectablePiecesRectangleSize = Pht::Vec2{6.8f, 2.4f} * GameHud::selectablePiecesScale;
    constexpr auto selectablePiecesRectangleTilt = 0.6f * GameHud::selectablePiecesScale;
    constexpr auto activeSlotXPosition = 0.0f;
    constexpr auto activeSlotXSize = 2.45f;
    constexpr auto dividerLineThickness = 0.02f;
    constexpr auto borderThickness = 0.055f;
    constexpr auto piecesRectangleOuterCornerRadius = 0.25f;
    constexpr auto pauseButtonRectangleTilt = 0.375f;

    std::unique_ptr<Pht::SoftwareRasterizer> CreateRasterizer(Pht::IEngine& engine,
                                                              const CommonResources& commonResources,
                                                              const Pht::Vec2& size) {
        auto& renderer = engine.GetRenderer();
        auto& renderBufferSize = renderer.GetRenderBufferSize();
        auto& frustumSize = commonResources.GetHudFrustumSizePotentiallyZoomedScreen();
        
        auto xScaleFactor =
            static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x);

        auto yScaleFactor =
            static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y);
        
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
                                            {0.0f, 0.0f, 0.0f, 0.0f});
    mPressedPauseButtonRectangle = CreateRectangle(engine,
                                                   commonResources,
                                                   pauseButtonRectangleSize,
                                                   pauseButtonRectangleTilt,
                                                   pressedColorSubtract);
    mNextPiecesRectangle = CreateRectangle(engine,
                                           commonResources,
                                           nextPiecesRectangleSize,
                                           nextPiecesRectangleTilt,
                                           {0.0f, 0.0f, 0.0f, 0.0f});
    mSelectablePiecesRectangle = CreateSelectablesRectangle(engine,
                                                            commonResources,
                                                            selectablePiecesRectangleSize,
                                                            selectablePiecesRectangleTilt,
                                                            {0.0f, 0.0f, 0.0f, 0.0f});
    mPressedSelectablePiecesRectangle = CreateSelectablesRectangle(engine,
                                                                   commonResources,
                                                                   selectablePiecesRectangleSize,
                                                                   selectablePiecesRectangleTilt,
                                                                   pressedColorSubtract);
}

std::unique_ptr<Pht::RenderableObject>
GameHudRectangles::CreateRectangle(Pht::IEngine& engine,
                                   const CommonResources& commonResources,
                                   const Pht::Vec2& size,
                                   float tilt,
                                   const Pht::Vec4& colorSubtract) {
    auto rasterizer = CreateRasterizer(engine, commonResources, size);

    DrawRectangleBorder(*rasterizer, size, colorSubtract);
    FillStencilBuffer(*rasterizer,
                      size,
                      piecesRectangleOuterCornerRadius - borderThickness * 2.0f,
                      borderThickness * 2.0f);
    DrawRectangleMainArea(*rasterizer, size, colorSubtract);
    
    auto image = rasterizer->ProduceImage();
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    Pht::QuadMesh quadMesh {size.x, size.y, tilt};
    return engine.GetSceneManager().CreateRenderableObject(quadMesh, imageMaterial);
}

std::unique_ptr<Pht::RenderableObject>
GameHudRectangles::CreateSelectablesRectangle(Pht::IEngine& engine,
                                              const CommonResources& commonResources,
                                              const Pht::Vec2& size,
                                              float tilt,
                                              const Pht::Vec4& colorSubtract) {
    auto rasterizer = CreateRasterizer(engine, commonResources, size);

    DrawSelectablesRectangleBorder(*rasterizer, size, colorSubtract);
    DrawActiveSlotArea(*rasterizer, size, colorSubtract);
    DrawSelectablesRectangleMainArea(*rasterizer, size, colorSubtract);
    DrawLine(*rasterizer, colorSubtract);

    auto image = rasterizer->ProduceImage();
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

void GameHudRectangles::DrawSelectablesRectangleBorder(Pht::SoftwareRasterizer& rasterizer,
                                                       const Pht::Vec2& size,
                                                       const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer, size, piecesRectangleOuterCornerRadius, 0.0f);
    
    Pht::SoftwareRasterizer::VerticalGradientColors outerBorderColors {
        PositiveSubtract(lowerOuterBorderColor, colorSubtract),
        PositiveSubtract(outerBorderColor, colorSubtract)
    };
    Pht::Vec2 lowerLeft {activeSlotXPosition + activeSlotXSize, 0.0f};
    Pht::Vec2 upperRight {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRight, lowerLeft, outerBorderColors, Pht::DrawOver::Yes);

    Pht::SoftwareRasterizer::VerticalGradientColors activeSlotOuterBorderColors {
        PositiveSubtract(activeSlotOuterLowerBorderColor, colorSubtract),
        PositiveSubtract(activeSlotOuterBorderColor, colorSubtract)
    };
    Pht::Vec2 activeSlotLowerLeft {0.0f, 0.0f};
    Pht::Vec2 activeSlotUpperRight {activeSlotXSize, size.y};
    rasterizer.DrawGradientRectangle(activeSlotUpperRight,
                                     activeSlotLowerLeft,
                                     activeSlotOuterBorderColors,
                                     Pht::DrawOver::Yes);
    
    FillStencilBuffer(rasterizer,
                      size,
                      piecesRectangleOuterCornerRadius - borderThickness,
                      borderThickness);

    Pht::SoftwareRasterizer::VerticalGradientColors borderColors {
        PositiveSubtract(lowerBorderColor, colorSubtract),
        PositiveSubtract(borderColor, colorSubtract)
    };
    Pht::Vec2 lowerLeft2 {activeSlotXPosition + activeSlotXSize, 0.0f};
    Pht::Vec2 upperRight2 {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRight2, lowerLeft2, borderColors, Pht::DrawOver::Yes);
    
    Pht::SoftwareRasterizer::VerticalGradientColors activeSlotBorderColors {
        PositiveSubtract(activeSlotInnerLowerBorderColor, colorSubtract),
        PositiveSubtract(activeSlotInnerBorderColor, colorSubtract)
    };
    Pht::Vec2 activeSlotLowerLeft2 {0.0f, 0.0f};
    Pht::Vec2 activeSlotUpperRight2 {activeSlotXSize, size.y};
    rasterizer.DrawGradientRectangle(activeSlotUpperRight2,
                                     activeSlotLowerLeft2,
                                     activeSlotBorderColors,
                                     Pht::DrawOver::Yes);
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

void GameHudRectangles::DrawSelectablesRectangleMainArea(Pht::SoftwareRasterizer& rasterizer,
                                                         const Pht::Vec2& size,
                                                         const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer,
                      size,
                      piecesRectangleOuterCornerRadius - borderThickness * 2.0f,
                      borderThickness * 2.0f);

    Pht::SoftwareRasterizer::VerticalGradientColors upperRectangleColors {
        PositiveSubtract(lowerInnerColor, colorSubtract),
        PositiveSubtract(innerColor, colorSubtract)
    };
    Pht::Vec2 lowerLeft2 {activeSlotXPosition + activeSlotXSize, 0.0f};
    Pht::Vec2 upperRight2 {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRight2,
                                     lowerLeft2,
                                     upperRectangleColors,
                                     Pht::DrawOver::Yes);
}

void GameHudRectangles::DrawActiveSlotArea(Pht::SoftwareRasterizer& rasterizer,
                                           const Pht::Vec2& size,
                                           const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer,
                      size,
                      piecesRectangleOuterCornerRadius - borderThickness * 2.0f,
                      borderThickness * 2.0f);

    Pht::SoftwareRasterizer::VerticalGradientColors upperRectangleColors {
        PositiveSubtract(activeSlotLowerInnerColor, colorSubtract),
        PositiveSubtract(activeSlotInnerColor, colorSubtract)
    };
    Pht::Vec2 lowerLeftFill {activeSlotXPosition, 0.0f};
    Pht::Vec2 upperRightFill {activeSlotXPosition + activeSlotXSize, size.y};
    rasterizer.DrawGradientRectangle(upperRightFill,
                                     lowerLeftFill,
                                     upperRectangleColors,
                                     Pht::DrawOver::Yes);

    FillStencilBuffer(rasterizer,
                      size,
                      piecesRectangleOuterCornerRadius - borderThickness,
                      borderThickness);

    Pht::Vec2 lowerLeft {activeSlotXPosition + 0.6f, gridLineWidth};
    Pht::Vec2 upperRight {activeSlotXPosition + 0.6f + gridLineWidth, size.y - borderThickness};
    rasterizer.SetBlend(Pht::Blend::Yes);
    rasterizer.DrawRectangle(upperRight, lowerLeft, gridLineColor, Pht::DrawOver::Yes);
    rasterizer.SetBlend(Pht::Blend::No);
}

void GameHudRectangles::DrawLine(Pht::SoftwareRasterizer& rasterizer,
                                 const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer,
                      selectablePiecesRectangleSize,
                      piecesRectangleOuterCornerRadius,
                      0.0f);

    auto dividerLineXPosition = activeSlotXPosition + activeSlotXSize;
    
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
