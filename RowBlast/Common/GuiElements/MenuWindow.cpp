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
    const Pht::Vec4 grayColor {0.91f, 0.91f, 0.93f, 1.0f};
    const Pht::Vec4 darkerGrayColor {0.88f, 0.88f, 0.90f, 1.0f};
    const Pht::Vec4 footerBorderColor {0.86f, 0.86f, 0.87f, 1.0f};
    const Pht::Vec4 blueColor {0.45f, 0.75f, 1.0f, 1.0};
    const Pht::Vec4 lightBlueColor {0.5f, 0.8f, 1.0f, 1.0};
    const Pht::Vec4 stencilColor {1.0f, 1.0f, 1.0f, 1.0f};
    const Pht::Vec4 borderColor {0.5f, 0.8f, 1.0f, 1.0};
    const Pht::Vec4 darkBlueColor {0.0f, 0.0f, 0.5f, 1.0};
    constexpr auto xBorder {0.45f};
    constexpr auto darkBorderThickness {0.2f};
    constexpr auto outerCornerRadius {0.37f};
    constexpr auto captionBarHeight {3.0f};
    constexpr auto squareSide {0.5f};
    constexpr auto footerBarHeight {2.0f};
    constexpr auto footerBarBorderHeight {0.075f};
}

MenuWindow::MenuWindow(Pht::IEngine& engine,
                       const CommonResources& commonResources,
                       Size size,
                       Style style,
                       PotentiallyZoomedScreen potentiallyZoomed) {
    auto& renderer {engine.GetRenderer()};
    auto& renderBufferSize {renderer.GetRenderBufferSize()};
    
    auto& frustumSize {
        potentiallyZoomed == PotentiallyZoomedScreen::Yes ?
        commonResources.GetHudFrustumSizePotentiallyZoomedScreen() : renderer.GetHudFrustumSize()
    };
    
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
            mSize = {sizeX, 11.0f};
            break;
    }
    
    Pht::IVec2 imageSize {
        static_cast<int>(mSize.x * xScaleFactor),
        static_cast<int>(mSize.y * yScaleFactor)
    };
    
    auto rasterizer {std::make_unique<Pht::OfflineRasterizer>(mSize, imageSize)};

    switch (style) {
        case Style::Bright:
            FillStencilBuffer(*rasterizer, outerCornerRadius, 0.0f);
            DrawBrightCaptionBar(*rasterizer);
            DrawBrightMainArea(*rasterizer);
            break;
        case Style::Dark:
            FillStencilBuffer(*rasterizer, outerCornerRadius, 0.0f);
            DrawDarkBorder(*rasterizer);
            FillStencilBuffer(*rasterizer,
                              outerCornerRadius - darkBorderThickness,
                              darkBorderThickness);
            DrawDarkMainArea(*rasterizer);
            break;
    }

    auto image {rasterizer->ProduceImage()};
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    auto& sceneManager {engine.GetSceneManager()};
    mRenderableObject = sceneManager.CreateRenderableObject(Pht::QuadMesh {mSize.x, mSize.y},
                                                            imageMaterial);
}

void MenuWindow::FillStencilBuffer(Pht::OfflineRasterizer& rasterizer,
                                   float cornerRadius,
                                   float padding) {
    rasterizer.SetStencilBufferFillMode();
    
    rasterizer.DrawCircle({cornerRadius + padding, cornerRadius + padding},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    rasterizer.DrawCircle({mSize.x - cornerRadius - padding, cornerRadius + padding},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    rasterizer.DrawCircle({mSize.x - cornerRadius - padding, mSize.y - cornerRadius - padding},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    rasterizer.DrawCircle({cornerRadius + padding, mSize.y - cornerRadius - padding},
                          cornerRadius,
                          cornerRadius,
                          stencilColor);
    
    Pht::Vec2 lowerLeft1 {cornerRadius + padding, padding};
    Pht::Vec2 upperRight1 {mSize.x - cornerRadius - padding, mSize.y - padding};
    rasterizer.DrawRectangle(upperRight1, lowerLeft1, stencilColor);

    Pht::Vec2 lowerLeft2 {padding, cornerRadius + padding};
    Pht::Vec2 upperRight2 {cornerRadius + padding, mSize.y - cornerRadius - padding};
    rasterizer.DrawRectangle(upperRight2, lowerLeft2, stencilColor);

    Pht::Vec2 lowerLeft3 {mSize.x - cornerRadius - padding, cornerRadius + padding};
    Pht::Vec2 upperRight3 {mSize.x - padding, mSize.y - cornerRadius - padding};
    rasterizer.DrawRectangle(upperRight3, lowerLeft3, stencilColor);

    rasterizer.EnableStencilTest();
}

void MenuWindow::DrawBrightCaptionBar(Pht::OfflineRasterizer& rasterizer) {
    Pht::OfflineRasterizer::HorizontalGradientColors rectangleColors {blueColor, lightBlueColor};
    Pht::Vec2 lowerLeft1 {0.0f, mSize.y - captionBarHeight};
    Pht::Vec2 upperRight1 {mSize.x, mSize.y};
    rasterizer.DrawGradientRectangle(upperRight1, lowerLeft1, rectangleColors, Pht::DrawOver::Yes);
    
    auto xStart {-squareSide / 2.0f};
    
    for (auto y {mSize.y - captionBarHeight + squareSide / 2.0f};
         y < mSize.y + squareSide;
         y += squareSide) {
        
        xStart -= squareSide;
        
        for (auto x {xStart}; x < mSize.x + squareSide; x += squareSide * 2.0f) {
            Pht::Vec2 lowerLeft {x - squareSide / 2.0f, y -  squareSide / 2.0f};
            Pht::Vec2 upperRight {x + squareSide / 2.0f, y +  squareSide / 2.0f};
            rasterizer.DrawRectangle(upperRight, lowerLeft, lightBlueColor, Pht::DrawOver::Yes);
        }
    }
}

void MenuWindow::DrawBrightMainArea(Pht::OfflineRasterizer& rasterizer) {
    Pht::OfflineRasterizer::HorizontalGradientColors rectangleColors {darkerGrayColor, grayColor};
    Pht::Vec2 lowerLeft1 {0.0f, 0.0f};
    Pht::Vec2 upperRight1 {mSize.x, mSize.y - captionBarHeight};
    rasterizer.DrawGradientRectangle(upperRight1, lowerLeft1, rectangleColors, Pht::DrawOver::Yes);
    
    auto xStart {-squareSide - squareSide / 2.0f};
    
    for (auto y {mSize.y - captionBarHeight - squareSide / 2.0f};
         y > -squareSide;
         y -= squareSide) {

        xStart -= squareSide;
        
        for (auto x {xStart}; x < mSize.x + squareSide; x += squareSide * 2.0f) {
            Pht::Vec2 lowerLeft {x - squareSide / 2.0f, y -  squareSide / 2.0f};
            Pht::Vec2 upperRight {x + squareSide / 2.0f, y +  squareSide / 2.0f};
            rasterizer.DrawRectangle(upperRight, lowerLeft, grayColor, Pht::DrawOver::Yes);
        }
    }
    
    Pht::Vec2 lowerLeft2 {0.0f, footerBarHeight};
    Pht::Vec2 upperRight2 {mSize.x, footerBarHeight + footerBarBorderHeight};
    rasterizer.DrawRectangle(upperRight2, lowerLeft2, footerBorderColor, Pht::DrawOver::Yes);
}

void MenuWindow::DrawDarkBorder(Pht::OfflineRasterizer& rasterizer) {
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {mSize.x, mSize.y};
    rasterizer.DrawRectangle(upperRight, lowerLeft, borderColor, Pht::DrawOver::Yes);
}

void MenuWindow::DrawDarkMainArea(Pht::OfflineRasterizer& rasterizer) {
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {mSize.x, mSize.y};
    rasterizer.DrawRectangle(upperRight, lowerLeft, darkBlueColor, Pht::DrawOver::Yes);
}
