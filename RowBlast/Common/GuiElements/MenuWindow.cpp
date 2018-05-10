#include "MenuWindow.hpp"

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
    const Pht::Vec4 grayColor {0.89f, 0.87f, 0.87f, 1.0f};
    const Pht::Vec4 darkerGrayColor {0.86f, 0.84f, 0.84f, 1.0f};
    const Pht::Vec4 footerBorderColor {0.84f, 0.83f, 0.83f, 1.0f};
    const Pht::Vec4 blueColor {0.45f, 0.75f, 1.0f, 1.0};
    const Pht::Vec4 lightBlueColor {0.5f, 0.8f, 1.0f, 1.0};
    const Pht::Vec4 stencilColor {1.0f, 1.0f, 1.0f, 1.0f};
    constexpr auto xBorder {0.45f};
    constexpr auto cornerRadius {0.37f};
    constexpr auto captionBarHeight {3.0f};
    constexpr auto captionBarCircleRadius {0.17f};
    constexpr auto captionBarCircleSpacing {0.5f};
    constexpr auto footerBarHeight {2.0f};
    constexpr auto footerBarBorderHeight {0.075f};
}

MenuWindow::MenuWindow(Pht::IEngine& engine, const CommonResources& commonResources, Size size) {
    auto& renderer {engine.GetRenderer()};
    auto& renderBufferSize {renderer.GetRenderBufferSize()};
    auto& frustumSize {commonResources.GetHudFrustumSizePotentiallyZoomedScreen()};
    
    auto xScaleFactor {static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)};
    auto yScaleFactor {static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)};
    
    auto sizeX {std::min(frustumSize.x - xBorder * 2.0f, 15.0f - xBorder * 2.0f)};
    
    switch (size) {
        case Size::Large:
            mSize = {sizeX, 19.0f};
            break;
        case Size::Medium:
            mSize = {sizeX, 14.0f};
            break;
        case Size::Small:
            mSize = {sizeX, 7.0f};
            break;
    }
    
    Pht::IVec2 imageSize {
        static_cast<int>(mSize.x * xScaleFactor),
        static_cast<int>(mSize.y * yScaleFactor)
    };
    
    auto rasterizer {std::make_unique<Pht::OfflineRasterizer>(mSize, imageSize)};

    FillStencilBuffer(*rasterizer);

    DrawCaptionBar(*rasterizer);
    DrawMainArea(*rasterizer);

    auto image {rasterizer->ProduceImage()};
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    auto& sceneManager {engine.GetSceneManager()};
    mRenderableObject = sceneManager.CreateRenderableObject(Pht::QuadMesh {mSize.x, mSize.y},
                                                            imageMaterial);
}

void MenuWindow::FillStencilBuffer(Pht::OfflineRasterizer& rasterizer) {
    rasterizer.SetStencilBufferFillMode();
    
    rasterizer.DrawCircle({cornerRadius, cornerRadius}, cornerRadius, cornerRadius, stencilColor);
    rasterizer.DrawCircle({mSize.x - cornerRadius, cornerRadius},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    rasterizer.DrawCircle({mSize.x - cornerRadius, mSize.y - cornerRadius},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    rasterizer.DrawCircle({cornerRadius, mSize.y - cornerRadius},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    
    Pht::Vec2 lowerLeft1 {cornerRadius, 0.0f};
    Pht::Vec2 upperRight1 {mSize.x - cornerRadius, mSize.y};
    rasterizer.DrawRectangle(upperRight1, lowerLeft1, stencilColor);

    Pht::Vec2 lowerLeft2 {0.0f, cornerRadius};
    Pht::Vec2 upperRight2 {cornerRadius, mSize.y - cornerRadius};
    rasterizer.DrawRectangle(upperRight2, lowerLeft2, stencilColor);

    Pht::Vec2 lowerLeft3 {mSize.x - cornerRadius, cornerRadius};
    Pht::Vec2 upperRight3 {mSize.x, mSize.y - cornerRadius};
    rasterizer.DrawRectangle(upperRight3, lowerLeft3, stencilColor);

    rasterizer.EnableStencilTest();
}

void MenuWindow::DrawCaptionBar(Pht::OfflineRasterizer& rasterizer) {
    Pht::OfflineRasterizer::HorizontalGradientColors rectangleColors {blueColor, lightBlueColor};
    Pht::Vec2 lowerLeft1 {0.0f, mSize.y - captionBarHeight};
    Pht::Vec2 upperRight1 {mSize.x, mSize.y};
    rasterizer.DrawGradientRectangle(upperRight1, lowerLeft1, rectangleColors, Pht::DrawOver::Yes);
    
    auto xStart {0.0f};
    
    for (auto y {mSize.y - captionBarHeight + captionBarCircleRadius + captionBarCircleSpacing / 5.0f};
         y < mSize.y + captionBarCircleSpacing;
         y += captionBarCircleSpacing) {
        
        xStart -= captionBarCircleSpacing / 2.0f;
        
        for (auto x {xStart}; x < mSize.x + captionBarCircleSpacing; x += captionBarCircleSpacing) {
            rasterizer.DrawCircle({x, y},
                                  captionBarCircleRadius,
                                  captionBarCircleRadius,
                                  lightBlueColor,
                                  Pht::DrawOver::Yes);
        }
    }
}

void MenuWindow::DrawMainArea(Pht::OfflineRasterizer& rasterizer) {
    Pht::OfflineRasterizer::HorizontalGradientColors rectangleColors {darkerGrayColor, grayColor};
    Pht::Vec2 lowerLeft1 {0.0f, 0.0f};
    Pht::Vec2 upperRight1 {mSize.x, mSize.y - captionBarHeight};
    rasterizer.DrawGradientRectangle(upperRight1, lowerLeft1, rectangleColors, Pht::DrawOver::Yes);
    
    Pht::Vec2 lowerLeft2 {0.0f, footerBarHeight};
    Pht::Vec2 upperRight2 {mSize.x, footerBarHeight + footerBarBorderHeight};
    rasterizer.DrawRectangle(upperRight2, lowerLeft2, footerBorderColor, Pht::DrawOver::Yes);

    auto xStart {0.0f};
    
    for (auto y {mSize.y - captionBarHeight - captionBarCircleRadius - captionBarCircleSpacing / 5.0f};
         y > -captionBarCircleSpacing;
         y -= captionBarCircleSpacing) {

        xStart -= captionBarCircleSpacing / 2.0f;
        
        for (auto x {xStart}; x < mSize.x + captionBarCircleSpacing; x += captionBarCircleSpacing) {
            rasterizer.DrawCircle({x, y},
                                  captionBarCircleRadius,
                                  captionBarCircleRadius,
                                  grayColor,
                                  Pht::DrawOver::Yes);
        }
    }
}
