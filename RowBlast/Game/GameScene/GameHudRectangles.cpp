#include "GameHudRectangles.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "ISceneManager.hpp"
#include "OfflineRasterizer.hpp"
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
    const Pht::Vec4 darkerColor {0.9f, 0.9f, 1.0f, 0.0f};

    const Pht::Vec2 piecesRectangleSize {4.8f, 2.4f};
    constexpr auto borderThickness {0.055f};
    constexpr auto piecesRectangleOuterCornerRadius {0.25f};
    constexpr auto piecesRectangleTilt {0.6f};

    std::unique_ptr<Pht::OfflineRasterizer> CreateRasterizer(Pht::IEngine& engine,
                                                             const CommonResources& commonResources,
                                                             const Pht::Vec2 size) {
        auto& renderer {engine.GetRenderer()};
        auto& renderBufferSize {renderer.GetRenderBufferSize()};
        auto& frustumSize {commonResources.GetHudFrustumSizePotentiallyZoomedScreen()};
        
        auto xScaleFactor {static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)};
        auto yScaleFactor {static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)};
        
        Pht::IVec2 imageSize {
            static_cast<int>(size.x * xScaleFactor),
            static_cast<int>(size.y * yScaleFactor)
        };
    
        return std::make_unique<Pht::OfflineRasterizer>(size, imageSize);
    }
}

GameHudRectangles::GameHudRectangles(Pht::IEngine& engine, const CommonResources& commonResources) {
    CreatePiecesRectangle(engine, commonResources);
}

void GameHudRectangles::CreatePiecesRectangle(Pht::IEngine& engine,
                                              const CommonResources& commonResources) {
    auto rasterizer {CreateRasterizer(engine, commonResources, piecesRectangleSize)};

    DrawPiecesRectangleBorder(*rasterizer, piecesRectangleSize);
    FillStencilBuffer(*rasterizer,
                      piecesRectangleSize,
                      piecesRectangleOuterCornerRadius - borderThickness * 2.0f,
                      borderThickness * 2.0f);
    DrawPiecesRectangleMainArea(*rasterizer, piecesRectangleSize);

    auto image {rasterizer->ProduceImage()};
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    Pht::QuadMesh quadMesh {piecesRectangleSize.x, piecesRectangleSize.y, piecesRectangleTilt};
    auto& sceneManager {engine.GetSceneManager()};
    mPiecesRectangleRenderable = sceneManager.CreateRenderableObject(quadMesh, imageMaterial);
}

void GameHudRectangles::FillStencilBuffer(Pht::OfflineRasterizer& rasterizer,
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

void GameHudRectangles::DrawPiecesRectangleBorder(Pht::OfflineRasterizer& rasterizer,
                                                  const Pht::Vec2& size) {
    FillStencilBuffer(rasterizer, size, piecesRectangleOuterCornerRadius, 0.0f);
    
    Pht::OfflineRasterizer::VerticalGradientColors outerBorderColors {
        lowerOuterBorderColor,
        outerBorderColor
    };
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRight, lowerLeft, outerBorderColors, Pht::DrawOver::Yes);

    FillStencilBuffer(rasterizer,
                      size,
                      piecesRectangleOuterCornerRadius - borderThickness,
                      borderThickness);

    Pht::OfflineRasterizer::VerticalGradientColors borderColors {
        lowerBorderColor,
        borderColor
    };
    Pht::Vec2 lowerLeft2 {0.0f, 0.0f};
    Pht::Vec2 upperRight2 {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRight2, lowerLeft2, borderColors, Pht::DrawOver::Yes);
}

void GameHudRectangles::DrawPiecesRectangleMainArea(Pht::OfflineRasterizer& rasterizer,
                                                    const Pht::Vec2& size) {
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {size.x, size.y};
    rasterizer.DrawRectangle(upperRight, lowerLeft, darkerColor, Pht::DrawOver::Yes);
    
    Pht::OfflineRasterizer::VerticalGradientColors upperRectangleColors {darkerColor, innerColor};
    Pht::Vec2 lowerLeft2 {0.0f, 0.0f};
    Pht::Vec2 upperRight2 {size.x, size.y};
    rasterizer.DrawGradientRectangle(upperRight2,
                                     lowerLeft2,
                                     upperRectangleColors,
                                     Pht::DrawOver::Yes);
}
