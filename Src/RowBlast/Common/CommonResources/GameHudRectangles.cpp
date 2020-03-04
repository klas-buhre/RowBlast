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
    const Pht::Vec4 stencilColor {1.0f, 1.0f, 1.0f, 1.0f};
    const Pht::Vec4 pressedColorSubtract {0.1f, 0.1f, 0.1f, 0.0f};
#if 1
/*
    const Pht::Vec4 outerBorderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 lowerOuterBorderColor {0.175f, 0.175f, 0.175f, 0.1f};
    const Pht::Vec4 borderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 lowerBorderColor {0.175f, 0.175f, 0.175f, 0.1f};
    const Pht::Vec4 innerColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 lowerInnerColor {0.175f, 0.175f, 0.175f, 0.1f};
*/
/*
    const Pht::Vec4 outerBorderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 lowerOuterBorderColor {0.175f, 0.175f, 0.175f, 0.5f};
    const Pht::Vec4 borderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 lowerBorderColor {0.175f, 0.175f, 0.175f, 0.5f};
    const Pht::Vec4 innerColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 lowerInnerColor {0.175f, 0.175f, 0.175f, 0.5f};
*/
    auto nextRecangleUpperAlpha = 0.75f;
    auto nextRecangleLowerAlpha = 0.5f;
    const Pht::Vec4 outerBorderColor {0.175f, 0.175f, 0.175f, nextRecangleUpperAlpha};
    const Pht::Vec4 lowerOuterBorderColor {0.175f, 0.175f, 0.175f, nextRecangleLowerAlpha};
    const Pht::Vec4 borderColor {0.175f, 0.175f, 0.175f, nextRecangleUpperAlpha};
    const Pht::Vec4 lowerBorderColor {0.175f, 0.175f, 0.175f, nextRecangleLowerAlpha};
    const Pht::Vec4 innerColor {0.175f, 0.175f, 0.175f, nextRecangleUpperAlpha};
    const Pht::Vec4 lowerInnerColor {0.175f, 0.175f, 0.175f, nextRecangleLowerAlpha};

/*
    constexpr auto f = 0.9f;
    constexpr auto activeSlotLowerAlpha = 0.66f;
    const Pht::Vec4 activeSlotInnerColor {0.53f * f, 0.2625f * f, 0.6085f * f, 0.98f};
    const Pht::Vec4 activeSlotLowerInnerColor {
        activeSlotInnerColor.ToVec3(), activeSlotInnerColor.w * activeSlotLowerAlpha
    };
    const Pht::Vec4 activeSlotOuterBorderColor {0.7f, 0.525f, 0.825f, 0.85f};
    const Pht::Vec4 activeSlotOuterLowerBorderColor {
        activeSlotOuterBorderColor.ToVec3(), activeSlotOuterBorderColor.w * activeSlotLowerAlpha
    };
    const auto activeSlotInnerBorderColorAdd =
        Pht::Vec4{activeSlotOuterBorderColor.ToVec3(), 0.0f} * 0.175f;
    const auto activeSlotInnerBorderColor = activeSlotInnerColor + activeSlotInnerBorderColorAdd;
    const auto activeSlotInnerLowerBorderColor =
        activeSlotLowerInnerColor + activeSlotInnerBorderColorAdd;

    // constexpr auto selectablesSlotLowerAlpha = 0.4f;
    constexpr auto selectablesSlotLowerAlpha = 0.66f;
    constexpr auto selectablesSlotLowerBorderAlpha = 0.5f;
    const Pht::Vec4 selectablesSlotInnerColor {0.53f * f, 0.2625f * f, 0.6085f * f, 0.96f};
    const Pht::Vec4 selectablesSlotLowerInnerColor {
        selectablesSlotInnerColor.ToVec3(), selectablesSlotInnerColor.w * selectablesSlotLowerAlpha
    };
    const Pht::Vec4 selectablesSlotOuterBorderColor {0.7f, 0.525f, 0.825f, 0.85f};
    const Pht::Vec4 selectablesSlotOuterLowerBorderColor {
        selectablesSlotOuterBorderColor.ToVec3(),
        selectablesSlotOuterBorderColor.w * selectablesSlotLowerBorderAlpha
    };
    const auto selectablesSlotInnerBorderColorAdd =
        Pht::Vec4{selectablesSlotOuterBorderColor.ToVec3(), 0.0f} * 0.175f;
    const auto selectablesSlotInnerBorderColor =
        selectablesSlotInnerColor + selectablesSlotInnerBorderColorAdd;
    const auto selectablesSlotInnerLowerBorderColor =
        selectablesSlotLowerInnerColor + selectablesSlotInnerBorderColorAdd;
*/
    // auto selectablesUpperAlpha = 0.75f;
    // auto selectablesLowerAlpha = 0.5f;
    auto selectablesUpperAlpha = 0.6f;
    auto selectablesLowerAlpha = 0.95f;

    const Pht::Vec4 activeSlotInnerColor {0.175f, 0.175f, 0.175f, selectablesUpperAlpha};
    const Pht::Vec4 activeSlotLowerInnerColor {0.175f, 0.175f, 0.175f, selectablesLowerAlpha};
    const Pht::Vec4 activeSlotOuterBorderColor {0.175f, 0.175f, 0.175f, selectablesUpperAlpha};
    const Pht::Vec4 activeSlotOuterLowerBorderColor {0.175f, 0.175f, 0.175f, selectablesLowerAlpha};
    const Pht::Vec4 activeSlotInnerBorderColor {0.175f, 0.175f, 0.175f, selectablesUpperAlpha};
    const Pht::Vec4 activeSlotInnerLowerBorderColor {0.175f, 0.175f, 0.175f, selectablesLowerAlpha};

    const Pht::Vec4 selectablesSlotInnerColor {0.175f, 0.175f, 0.175f, selectablesUpperAlpha};
    const Pht::Vec4 selectablesSlotLowerInnerColor {0.175f, 0.175f, 0.175f, selectablesLowerAlpha};
    const Pht::Vec4 selectablesSlotOuterBorderColor {0.175f, 0.175f, 0.175f, selectablesUpperAlpha};;
    const Pht::Vec4 selectablesSlotOuterLowerBorderColor {0.175f, 0.175f, 0.175f, selectablesLowerAlpha};
    const Pht::Vec4 selectablesSlotInnerBorderColor {0.175f, 0.175f, 0.175f, selectablesUpperAlpha};
    const Pht::Vec4 selectablesSlotInnerLowerBorderColor {0.175f, 0.175f, 0.175f, selectablesLowerAlpha};

/*
    const Pht::Vec4 activeSlotInnerColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 activeSlotLowerInnerColor {0.175f, 0.175f, 0.175f, 0.5f};
    const Pht::Vec4 activeSlotOuterBorderColor {0.175f, 0.175f, 0.175f, 0.5f};
    const Pht::Vec4 activeSlotOuterLowerBorderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 activeSlotInnerBorderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 activeSlotInnerLowerBorderColor {0.175f, 0.175f, 0.175f, 0.6f};

    const Pht::Vec4 selectablesSlotInnerColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 selectablesSlotLowerInnerColor {0.175f, 0.175f, 0.175f, 0.5f};
    const Pht::Vec4 selectablesSlotOuterBorderColor {0.175f, 0.175f, 0.175f, 0.5f};;
    const Pht::Vec4 selectablesSlotOuterLowerBorderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 selectablesSlotInnerBorderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 selectablesSlotInnerLowerBorderColor {0.175f, 0.175f, 0.175f, 0.6f};
*/
#else
    const Pht::Vec4 outerBorderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 lowerOuterBorderColor {0.175f, 0.175f, 0.175f, 0.1f};
    const Pht::Vec4 borderColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 lowerBorderColor {0.175f, 0.175f, 0.175f, 0.1f};
    const Pht::Vec4 innerColor {0.175f, 0.175f, 0.175f, 0.75f};
    const Pht::Vec4 lowerInnerColor {0.175f, 0.175f, 0.175f, 0.1f};

    constexpr auto f = 0.9f;
    constexpr auto activeSlotLowerAlpha = 0.66f;
    const Pht::Vec4 activeSlotInnerColor {0.53f * f, 0.2625f * f, 0.6085f * f, 0.98f};
    const Pht::Vec4 activeSlotLowerInnerColor {
        activeSlotInnerColor.ToVec3(), activeSlotInnerColor.w * activeSlotLowerAlpha
    };
    const Pht::Vec4 activeSlotOuterBorderColor {0.7f, 0.525f, 0.825f, 0.85f};
    const Pht::Vec4 activeSlotOuterLowerBorderColor {
        activeSlotOuterBorderColor.ToVec3(), activeSlotOuterBorderColor.w * activeSlotLowerAlpha
    };
    const auto activeSlotInnerBorderColorAdd =
        Pht::Vec4{activeSlotOuterBorderColor.ToVec3(), 0.0f} * 0.175f;
    const auto activeSlotInnerBorderColor = activeSlotInnerColor + activeSlotInnerBorderColorAdd;
    const auto activeSlotInnerLowerBorderColor =
        activeSlotLowerInnerColor + activeSlotInnerBorderColorAdd;

    constexpr auto selectablesSlotLowerAlpha = 0.4f;
    constexpr auto selectablesSlotLowerBorderAlpha = 0.5f;
    const Pht::Vec4 selectablesSlotInnerColor {0.53f * f, 0.2625f * f, 0.6085f * f, 0.96f};
    const Pht::Vec4 selectablesSlotLowerInnerColor {
        selectablesSlotInnerColor.ToVec3(), selectablesSlotInnerColor.w * selectablesSlotLowerAlpha
    };
    const Pht::Vec4 selectablesSlotOuterBorderColor {0.7f, 0.525f, 0.825f, 0.85f};
    const Pht::Vec4 selectablesSlotOuterLowerBorderColor {
        selectablesSlotOuterBorderColor.ToVec3(),
        selectablesSlotOuterBorderColor.w * selectablesSlotLowerBorderAlpha
    };
    const auto selectablesSlotInnerBorderColorAdd =
        Pht::Vec4{selectablesSlotOuterBorderColor.ToVec3(), 0.0f} * 0.175f;
    const auto selectablesSlotInnerBorderColor =
        selectablesSlotInnerColor + selectablesSlotInnerBorderColorAdd;
    const auto selectablesSlotInnerLowerBorderColor =
        selectablesSlotLowerInnerColor + selectablesSlotInnerBorderColorAdd;
#endif

    // constexpr auto tiltFactor = 0.75f;
    constexpr auto tiltFactor = 0.0f;
    const Pht::Vec2 pauseButtonRectangleSize {1.5f, 1.5f};
    const Pht::Vec2 nextPiecesRectangleSize {
        3.35f * GameHud::nextPiecesContainerScale, 3.3f * GameHud::selectablePiecesContainerScale
    };
    const auto selectablePiecesRectangleSize =
        Pht::Vec2{8.5f, 2.75f} * GameHud::selectablePiecesContainerScale;
    constexpr auto selectablePiecesRectangleTilt =
        0.75f * GameHud::selectablePiecesContainerScale * tiltFactor;

    const auto gridCellSize = selectablePiecesRectangleSize.y / 4.0f;
    constexpr auto gridLineWidth = 0.04f;
    // const Pht::Vec4 gridLineColor {1.0f, 1.0f, 1.0f, 0.05f};
    const Pht::Vec4 gridLineColor {1.0f, 1.0f, 1.0f, 0.15f};

    constexpr auto activeSlotXPosition = 0.0f;
    constexpr auto activeSlotXSize = 3.3f;
    constexpr auto dividerLineThickness = 0.04f;
    constexpr auto borderThickness = 0.06f;
    constexpr auto piecesRectangleOuterCornerRadius = 0.25f;
    
    constexpr auto nextRectangleBarHeight = 0.72f;
    constexpr auto nextRectangleBarGlossyAreaHeight = 0.1f;
    
    const Pht::Vec4 transparentColor {0.0f, 0.0f, 0.0f, 0.0f};
    const Pht::Vec4 darkerColor {0.0f, 0.0f, 0.2f, 0.15f};

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
            static_cast<int>(size.x * xScaleFactor) * 2,
            static_cast<int>(size.y * yScaleFactor) * 2
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
    
    void FillStencilBuffer(Pht::SoftwareRasterizer& rasterizer,
                           const Pht::Vec2& size,
                           float cornerRadius,
                           float xPadding,
                           float upperYPadding,
                           float lowerYPadding) {
        rasterizer.SetStencilBufferFillMode();
        
        rasterizer.DrawCircle({cornerRadius + xPadding, cornerRadius + lowerYPadding},
                              cornerRadius,
                              cornerRadius,
                              stencilColor);
        rasterizer.DrawCircle({size.x - cornerRadius - xPadding, cornerRadius + lowerYPadding},
                              cornerRadius,
                              cornerRadius,
                              stencilColor);
        rasterizer.DrawCircle({size.x - cornerRadius - xPadding, size.y - cornerRadius - upperYPadding},
                              cornerRadius,
                              cornerRadius,
                              stencilColor);
        rasterizer.DrawCircle({cornerRadius + xPadding, size.y - cornerRadius - upperYPadding},
                              cornerRadius,
                              cornerRadius,
                              stencilColor);
        
        Pht::Vec2 lowerLeft1 {cornerRadius + xPadding, lowerYPadding};
        Pht::Vec2 upperRight1 {size.x - cornerRadius - xPadding, size.y - upperYPadding};
        rasterizer.DrawRectangle(upperRight1, lowerLeft1, stencilColor);

        Pht::Vec2 lowerLeft2 {xPadding, cornerRadius + lowerYPadding - 0.05f};
        Pht::Vec2 upperRight2 {cornerRadius + xPadding, size.y - cornerRadius - upperYPadding};
        rasterizer.DrawRectangle(upperRight2, lowerLeft2, stencilColor);

        Pht::Vec2 lowerLeft3 {size.x - cornerRadius - xPadding, cornerRadius + lowerYPadding - 0.05f};
        Pht::Vec2 upperRight3 {size.x - xPadding, size.y - cornerRadius - upperYPadding};
        rasterizer.DrawRectangle(upperRight3, lowerLeft3, stencilColor);

        rasterizer.EnableStencilTest();
    }
}

GameHudRectangles::GameHudRectangles(Pht::IEngine& engine, const CommonResources& commonResources) {
    mNextPiecesRectangle = CreateNextRectangle(engine, commonResources, nextPiecesRectangleSize);
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
GameHudRectangles::CreateNextRectangle(Pht::IEngine& engine,
                                       const CommonResources& commonResources,
                                       const Pht::Vec2& size) {
    auto rasterizer = CreateRasterizer(engine, commonResources, size);

    FillStencilBuffer(*rasterizer, size, piecesRectangleOuterCornerRadius, 0.0f, 0.0f, 0.0f);
    DrawNextRectangleMainArea(*rasterizer, size);
    DrawNextRectangleUpperBarGlossyArea(*rasterizer, size);
    DrawNextRectangleUpperBarGradientArea(*rasterizer, size);
    
    auto image = rasterizer->ProduceImage();
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    Pht::QuadMesh quadMesh {size.x, size.y};
    return engine.GetSceneManager().CreateRenderableObject(quadMesh, imageMaterial);
}

void GameHudRectangles::DrawNextRectangleUpperBarGradientArea(Pht::SoftwareRasterizer& rasterizer,
                                                              const Pht::Vec2& size) {
    FillStencilBuffer(rasterizer,
                      size,
                      piecesRectangleOuterCornerRadius,
                      0.0f,
                      nextRectangleBarGlossyAreaHeight,
                      0.0f);

    constexpr auto ft = 1.1f;
    constexpr auto fb = 1.2f;

    Pht::SoftwareRasterizer::VerticalGradientColors gradientColors {
        .mBottom = {0.0f, 0.285f * fb, 0.585f * fb, 1.0f},
        .mTop = {0.0f, 0.585f * ft, 0.935f * ft, 1.0f}
    };
    Pht::Vec2 lowerLeft {0.0f, size.y - nextRectangleBarHeight};
    Pht::Vec2 upperRight {size.x, size.y - nextRectangleBarGlossyAreaHeight};
    rasterizer.DrawGradientRectangle(upperRight, lowerLeft, gradientColors, Pht::DrawOver::Yes);
}

void GameHudRectangles::DrawNextRectangleUpperBarGlossyArea(Pht::SoftwareRasterizer& rasterizer,
                                                            const Pht::Vec2& size) {
    FillStencilBuffer(rasterizer, size, piecesRectangleOuterCornerRadius, 0.0f, 0.0f, 0.0f);

    constexpr auto f = 1.2f;
    Pht::Vec4 color {0.11f * f, 0.66f * f, 1.0f * f, 1.0f};
    Pht::Vec2 lowerLeft {0.0f, size.y - nextRectangleBarHeight};
    Pht::Vec2 upperRight {size.x, size.y};
    rasterizer.DrawRectangle(upperRight, lowerLeft, color, Pht::DrawOver::Yes);
}

std::unique_ptr<Pht::RenderableObject>
GameHudRectangles::CreateSelectablesRectangle(Pht::IEngine& engine,
                                              const CommonResources& commonResources,
                                              const Pht::Vec2& size,
                                              float tilt,
                                              const Pht::Vec4& colorSubtract) {
    auto rasterizer = CreateRasterizer(engine, commonResources, size);

    DrawSelectablesRectangleCheckedArea(*rasterizer, size, colorSubtract);
    // DrawSelectablesRectangleBorder(*rasterizer, size, colorSubtract);
    // DrawActiveSlotArea(*rasterizer, size, colorSubtract);
    // DrawSelectablesRectangleMainArea(*rasterizer, size, colorSubtract);
    // DrawLine(*rasterizer, colorSubtract);

    auto image = rasterizer->ProduceImage();
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::No};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    Pht::QuadMesh quadMesh {size.x, size.y, tilt};
    return engine.GetSceneManager().CreateRenderableObject(quadMesh, imageMaterial);
}

void GameHudRectangles::DrawSelectablesRectangleCheckedArea(Pht::SoftwareRasterizer& rasterizer,
                                                            const Pht::Vec2& size,
                                                            const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer, size, piecesRectangleOuterCornerRadius, 0.0f, 0.0f, 0.0f);

    Pht::SoftwareRasterizer::VerticalGradientColors upperRectangleColors {
        PositiveSubtract(activeSlotLowerInnerColor, colorSubtract),
        PositiveSubtract(activeSlotInnerColor, colorSubtract)
    };
    Pht::Vec2 lowerLeftFill {0.0f, 0.0f};
    Pht::Vec2 upperRightFill {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRightFill,
                                     lowerLeftFill,
                                     upperRectangleColors,
                                     Pht::DrawOver::Yes);
    
    rasterizer.SetBlend(Pht::Blend::Yes);
    
    for (auto row = 0;; ++row) {
        auto y = row * gridCellSize;
        if (y > size.y) {
            break;
        }
        
        auto counter = row % 2;
        for (auto column = 0;; ++column) {
            auto x = column * gridCellSize;
            if (x > size.x) {
                break;
            }
            
            Pht::Vec2 lowerLeft {x, y};
            Pht::Vec2 upperRight {x + gridCellSize, y + gridCellSize};
            auto color = counter % 2 ? darkerColor : transparentColor;
            rasterizer.DrawRectangle(upperRight, lowerLeft, color, Pht::DrawOver::Yes);
            ++counter;
        }
    }
}

void GameHudRectangles::DrawSelectablesRectangleBorder(Pht::SoftwareRasterizer& rasterizer,
                                                       const Pht::Vec2& size,
                                                       const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer, size, piecesRectangleOuterCornerRadius, 0.0f, 0.0f, 0.0f);
    
    Pht::SoftwareRasterizer::VerticalGradientColors outerBorderColors {
        PositiveSubtract(selectablesSlotOuterLowerBorderColor, colorSubtract),
        PositiveSubtract(selectablesSlotOuterBorderColor, colorSubtract)
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
                      borderThickness,
                      borderThickness,
                      borderThickness);

    Pht::SoftwareRasterizer::VerticalGradientColors borderColors {
        PositiveSubtract(selectablesSlotInnerLowerBorderColor, colorSubtract),
        PositiveSubtract(selectablesSlotInnerBorderColor, colorSubtract)
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

void GameHudRectangles::DrawNextRectangleMainArea(Pht::SoftwareRasterizer& rasterizer,
                                                  const Pht::Vec2& size) {
    Pht::SoftwareRasterizer::VerticalGradientColors upperRectangleColors {
        .mBottom = lowerInnerColor,
        .mTop = innerColor
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
                      borderThickness * 2.0f,
                      borderThickness * 2.0f,
                      borderThickness * 2.0f);

    Pht::SoftwareRasterizer::VerticalGradientColors upperRectangleColors {
        PositiveSubtract(selectablesSlotLowerInnerColor, colorSubtract),
        PositiveSubtract(selectablesSlotInnerColor, colorSubtract)
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
                      borderThickness * 2.0f,
                      borderThickness * 2.0f,
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
    DrawGrid(rasterizer,
             size,
             {activeSlotXPosition, 0.0f},
             // {activeSlotXSize, size.y},
             {size.x, size.y},
             colorSubtract);
}

void GameHudRectangles::DrawGrid(Pht::SoftwareRasterizer& rasterizer,
                                 const Pht::Vec2& rectangleSize,
                                 const Pht::Vec2& gridPosition,
                                 const Pht::Vec2& gridArea,
                                 const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer,
                      rectangleSize,
                      piecesRectangleOuterCornerRadius - borderThickness,
                      borderThickness,
                      borderThickness,
                      borderThickness);
    
    rasterizer.SetBlend(Pht::Blend::Yes);
    
    for (auto column = 0;; ++column) {
        auto x = column * gridCellSize + gridPosition.x;
        if (x >= gridPosition.x + gridArea.x) {
            break;
        }

        Pht::SoftwareRasterizer::VerticalGradientColors upperRectangleColors {
            PositiveSubtract(gridLineColor, colorSubtract),
            PositiveSubtract(transparentColor, colorSubtract)
        };
        Pht::Vec2 lowerLeft {x, gridPosition.y};
        Pht::Vec2 upperRight {x + gridLineWidth, gridPosition.y + gridArea.y};
        rasterizer.DrawGradientRectangle(upperRight,
                                         lowerLeft,
                                         upperRectangleColors,
                                         Pht::DrawOver::Yes);
    }

    for (auto row = 0;; ++row) {
        auto y = row * gridCellSize + gridPosition.y;
        if (y > gridPosition.y + gridArea.y) {
            break;
        }
        
        Pht::Vec2 lowerLeft {gridPosition.x, y};
        Pht::Vec2 upperRight {gridPosition.x + gridArea.x, y + gridLineWidth};
        Pht::Vec4 color {
            gridLineColor.x,
            gridLineColor.y,
            gridLineColor.z,
            gridLineColor.w * (1.0f - y / gridArea.y)
        };
        rasterizer.DrawRectangle(upperRight,
                                 lowerLeft,
                                 PositiveSubtract(color, colorSubtract),
                                 Pht::DrawOver::Yes);
    }

    rasterizer.SetBlend(Pht::Blend::No);
}
/*
void GameHudRectangles::DrawGrid(Pht::SoftwareRasterizer& rasterizer,
                                 const Pht::Vec2& rectangleSize,
                                 const Pht::Vec2& gridPosition,
                                 const Pht::Vec2& gridArea,
                                 const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer,
                      rectangleSize,
                      piecesRectangleOuterCornerRadius - borderThickness,
                      borderThickness);
    
    rasterizer.SetBlend(Pht::Blend::Yes);
    
    for (auto column = 0;; ++column) {
        auto x = column * gridCellSize + gridPosition.x;
        if (x >= gridPosition.x + gridArea.x) {
            break;
        }
        
        Pht::Vec2 lowerLeft {x, gridPosition.y};
        Pht::Vec2 upperRight {x + gridLineWidth, gridPosition.y + gridArea.y};
        rasterizer.DrawRectangle(upperRight,
                                 lowerLeft,
                                 PositiveSubtract(gridLineColor, colorSubtract),
                                 Pht::DrawOver::Yes);
    }

    for (auto row = 0;; ++row) {
        auto y = row * gridCellSize + gridPosition.y;
        if (y > gridPosition.y + gridArea.y) {
            break;
        }
        
        Pht::Vec2 lowerLeft {gridPosition.x, y};
        Pht::Vec2 upperRight {gridPosition.x + gridArea.x, y + gridLineWidth};
        rasterizer.DrawRectangle(upperRight,
                                 lowerLeft,
                                 PositiveSubtract(gridLineColor, colorSubtract),
                                 Pht::DrawOver::Yes);
    }

    rasterizer.SetBlend(Pht::Blend::No);
}
*/
void GameHudRectangles::DrawLine(Pht::SoftwareRasterizer& rasterizer,
                                 const Pht::Vec4& colorSubtract) {
    FillStencilBuffer(rasterizer,
                      selectablePiecesRectangleSize,
                      piecesRectangleOuterCornerRadius,
                      0.0f,
                      0.0f,
                      0.0f);

    auto dividerLineXPosition = activeSlotXPosition + activeSlotXSize - dividerLineThickness;

    Pht::SoftwareRasterizer::VerticalGradientColors outerLineColors {
        PositiveSubtract(activeSlotInnerLowerBorderColor, colorSubtract),
        PositiveSubtract(activeSlotInnerBorderColor, colorSubtract)
    };
    Pht::Vec2 lowerLeft1 {dividerLineXPosition, borderThickness};
    Pht::Vec2 upperRight1 {
        dividerLineXPosition + dividerLineThickness,
        selectablePiecesRectangleSize.y - borderThickness};
    rasterizer.DrawGradientRectangle(upperRight1, lowerLeft1, outerLineColors, Pht::DrawOver::Yes);
}
