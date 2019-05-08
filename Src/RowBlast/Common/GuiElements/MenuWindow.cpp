#include "MenuWindow.hpp"

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
    const Pht::Vec4 grayColor {0.96f, 0.95f, 0.92f, 1.0f};
    const Pht::Vec4 darkerGrayColor {0.94f, 0.93f, 0.9f, 1.0f};
    const Pht::Vec4 blueColor {0.45f, 0.75f, 1.0f, 1.0};
    const Pht::Vec4 lightBlueColor {0.5f, 0.8f, 1.0f, 1.0};
    const Pht::Vec4 stencilColor {1.0f, 1.0f, 1.0f, 1.0f};
    const Pht::Vec4 outerBorderGlowColor {0.325f, 0.65f, 1.0f, 0.35f};
    const Pht::Vec4 borderColor {0.325f, 0.65f, 1.0f, 1.0};
    const Pht::Vec4 innerBorderColor {0.2f, 0.35f, 0.5f, 1.0};
    const Pht::Vec4 darkerBlueColor {0.04f, 0.07f, 0.2f, 1.0};
    constexpr auto xBorder = 0.45f;
    constexpr auto darkBorderThickness = 0.08f;
    constexpr auto outerGlowBorderThickness = 0.09f;
    constexpr auto outerCornerRadius = 0.29f;
    constexpr auto largeCaptionBarHeight = 3.0f;
    constexpr auto smallCaptionBarHeight = 2.5f;
    constexpr auto squareSide = 0.5f;
    
    Pht::Vec2 CalcSize(MenuWindow::Size size,
                       MenuWindow::Style style,
                       const Pht::Vec2& frustumSize) {
        switch (style) {
            case MenuWindow::Style::Bright: {
                auto sizeX = std::min(frustumSize.x - xBorder * 2.0f, 13.5f - xBorder * 2.0f);
                switch (size) {
                    case MenuWindow::Size::Large:
                        return {sizeX, 22.4f};
                    case MenuWindow::Size::Medium:
                        return {sizeX, 11.8f};
                    case MenuWindow::Size::Small:
                        return {sizeX, 11.0f};
                    case MenuWindow::Size::Smallest:
                        return {sizeX, 5.5f};
                }
                break;
            }
            case MenuWindow::Style::Dark: {
                auto sizeX = std::min(frustumSize.x - xBorder * 2.0f, 14.0f - xBorder * 2.0f);
                Pht::Vec2 outerBorderSize {
                    outerGlowBorderThickness * 2.0f,
                    outerGlowBorderThickness * 2.0f
                };
                switch (size) {
                    case MenuWindow::Size::Large:
                        return Pht::Vec2 {sizeX, 20.0f} + outerBorderSize;
                    case MenuWindow::Size::Medium:
                        return Pht::Vec2 {sizeX, 13.4f} + outerBorderSize;
                    case MenuWindow::Size::Small:
                        return Pht::Vec2 {sizeX, 11.0f} + outerBorderSize;
                    case MenuWindow::Size::Smallest:
                        return Pht::Vec2 {sizeX, 5.5f} + outerBorderSize;
                }
                break;
            }
        }
        
        assert(!"Unsupported menu window size");
    }
}

MenuWindow::MenuWindow(Pht::IEngine& engine,
                       const CommonResources& commonResources,
                       Size size,
                       Style style) {
    auto& renderer = engine.GetRenderer();
    auto& renderBufferSize = renderer.GetRenderBufferSize();
    auto& frustumSize = commonResources.GetHudFrustumSizePotentiallyZoomedScreen();

    auto xScaleFactor = static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x);
    auto yScaleFactor = static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y);
    
    mSize = CalcSize(size, style, frustumSize);
    
    Pht::IVec2 imageSize {
        static_cast<int>(mSize.x * xScaleFactor),
        static_cast<int>(mSize.y * yScaleFactor)
    };
    
    auto rasterizer = std::make_unique<Pht::SoftwareRasterizer>(mSize, imageSize);

    switch (style) {
        case Style::Bright: {
            FillStencilBuffer(*rasterizer, outerCornerRadius, 0.0f);
            auto captionBarHeight =
                size == Size::Large ? largeCaptionBarHeight : smallCaptionBarHeight;
            DrawBrightCaptionBar(*rasterizer, captionBarHeight);
            DrawBrightMainArea(*rasterizer, captionBarHeight);
            break;
        }
        case Style::Dark:
            DrawDarkBorder(*rasterizer);
            FillStencilBuffer(*rasterizer,
                              outerCornerRadius - darkBorderThickness * 2.0f,
                              darkBorderThickness * 2.0f + outerGlowBorderThickness);
            DrawDarkMainArea(*rasterizer);
            break;
    }

    auto image = rasterizer->ProduceImage();
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    
    if (style == Style::Dark) {
        imageMaterial.SetOpacity(0.95f);
    }

    auto& sceneManager = engine.GetSceneManager();
    mRenderableObject = sceneManager.CreateRenderableObject(Pht::QuadMesh {mSize.x, mSize.y},
                                                            imageMaterial);
    
    if (style == Style::Dark) {
        mSize -= Pht::Vec2 {outerGlowBorderThickness * 2.0f, outerGlowBorderThickness * 2.0f};
    }
}

void MenuWindow::FillStencilBuffer(Pht::SoftwareRasterizer& rasterizer,
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

void MenuWindow::DrawBrightCaptionBar(Pht::SoftwareRasterizer& rasterizer, float captionBarHeight) {
    Pht::SoftwareRasterizer::HorizontalGradientColors rectangleColors {blueColor, lightBlueColor};
    Pht::Vec2 lowerLeft1 {0.0f, mSize.y - captionBarHeight};
    Pht::Vec2 upperRight1 {mSize.x, mSize.y};
    rasterizer.DrawGradientRectangle(upperRight1, lowerLeft1, rectangleColors, Pht::DrawOver::Yes);
    
    auto xStart = -squareSide / 2.0f;
    
    for (auto y = mSize.y - captionBarHeight + squareSide / 2.0f;
         y < mSize.y + squareSide;
         y += squareSide) {
        
        xStart -= squareSide;
        
        for (auto x = xStart; x < mSize.x + squareSide; x += squareSide * 2.0f) {
            Pht::Vec2 lowerLeft {x - squareSide / 2.0f, y - squareSide / 2.0f};
            Pht::Vec2 upperRight {x + squareSide / 2.0f, y + squareSide / 2.0f};
            rasterizer.DrawRectangle(upperRight, lowerLeft, lightBlueColor, Pht::DrawOver::Yes);
        }
    }
}

void MenuWindow::DrawBrightMainArea(Pht::SoftwareRasterizer& rasterizer, float captionBarHeight) {
    Pht::SoftwareRasterizer::HorizontalGradientColors rectangleColors {darkerGrayColor, grayColor};
    Pht::Vec2 lowerLeft1 {0.0f, 0.0f};
    Pht::Vec2 upperRight1 {mSize.x, mSize.y - captionBarHeight};
    rasterizer.DrawGradientRectangle(upperRight1, lowerLeft1, rectangleColors, Pht::DrawOver::Yes);
    
    auto xStart = -squareSide - squareSide / 2.0f;
    
    for (auto y = mSize.y - captionBarHeight - squareSide / 2.0f;
         y > -squareSide;
         y -= squareSide) {

        xStart -= squareSide;
        
        for (auto x = xStart; x < mSize.x + squareSide; x += squareSide * 2.0f) {
            Pht::Vec2 lowerLeft {x - squareSide / 2.0f, y - squareSide / 2.0f};
            Pht::Vec2 upperRight {x + squareSide / 2.0f, y + squareSide / 2.0f};
            rasterizer.DrawRectangle(upperRight, lowerLeft, grayColor, Pht::DrawOver::Yes);
        }
    }
}

void MenuWindow::DrawDarkBorder(Pht::SoftwareRasterizer& rasterizer) {
    FillStencilBuffer(rasterizer, outerCornerRadius + outerGlowBorderThickness, 0.0f);
    DrawBorder(rasterizer, outerBorderGlowColor);
    
    FillStencilBuffer(rasterizer, outerCornerRadius, outerGlowBorderThickness);
    DrawBorder(rasterizer, borderColor);

    FillStencilBuffer(rasterizer,
                      outerCornerRadius - darkBorderThickness,
                      darkBorderThickness + outerGlowBorderThickness);
    DrawBorder(rasterizer, innerBorderColor);
}

void MenuWindow::DrawBorder(Pht::SoftwareRasterizer& rasterizer, const Pht::Vec4& color) {
    Pht::Vec2 lowerLeft1 {0.0f, 0.0f};
    Pht::Vec2 upperRight1 {mSize.x, mSize.y};
    rasterizer.DrawRectangle(upperRight1, lowerLeft1, color, Pht::DrawOver::Yes);
}

void MenuWindow::DrawDarkMainArea(Pht::SoftwareRasterizer& rasterizer) {
    Pht::Vec2 lowerLeft {0.0f, 0.0f};
    Pht::Vec2 upperRight {mSize.x, mSize.y};
    rasterizer.DrawRectangle(upperRight, lowerLeft, darkerBlueColor, Pht::DrawOver::Yes);
    
    Pht::SoftwareRasterizer::VerticalGradientColors upperRectangleColors {
        darkerBlueColor,
        innerBorderColor
    };
    Pht::Vec2 lowerLeft2 {0.0f, mSize.y - mSize.y / 3.0f};
    Pht::Vec2 upperRight2 {mSize.x, mSize.y};
    rasterizer.DrawGradientRectangle(upperRight2,
                                     lowerLeft2,
                                     upperRectangleColors,
                                     Pht::DrawOver::Yes);

    Pht::SoftwareRasterizer::VerticalGradientColors lowerRectangleColors {
        innerBorderColor,
        darkerBlueColor
    };
    Pht::Vec2 lowerLeft3 {0.0f, 0.0f};
    Pht::Vec2 upperRight3 {mSize.x, mSize.y / 3.0f};
    rasterizer.DrawGradientRectangle(upperRight3,
                                     lowerLeft3,
                                     lowerRectangleColors,
                                     Pht::DrawOver::Yes);
}
