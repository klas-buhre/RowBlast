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
    const Pht::Vec4 whiteColor {0.95f, 0.94f, 0.94f, 1.0f};
    const Pht::Vec4 grayColor {0.935f, 0.925f, 0.925f, 1.0f};
    const Pht::Vec4 stencilColor {1.0f, 1.0f, 1.0f, 1.0f};
    constexpr auto stripeStep {0.35f};
    constexpr auto xBorder {0.45f};
    constexpr auto cornerRadius {0.38f};
    
    void FillStencilBuffer(Pht::OfflineRasterizer& rasterizer, const Pht::Vec2& menuSize) {
        rasterizer.SetStencilBufferFillMode();
        
        rasterizer.DrawCircle({cornerRadius, cornerRadius},
                              cornerRadius,
                              cornerRadius,
                              stencilColor);
        rasterizer.DrawCircle({menuSize.x - cornerRadius, cornerRadius},
                              cornerRadius,
                              cornerRadius,
                              stencilColor);
        rasterizer.DrawCircle({menuSize.x - cornerRadius, menuSize.y - cornerRadius},
                              cornerRadius,
                              cornerRadius,
                              stencilColor);
        rasterizer.DrawCircle({cornerRadius, menuSize.y - cornerRadius},
                              cornerRadius,
                              cornerRadius,
                              stencilColor);
        
        Pht::Vec2 lowerLeft1 {cornerRadius, 0.0f};
        Pht::Vec2 upperRight1 {menuSize.x - cornerRadius, menuSize.y};
        rasterizer.DrawRectangle(upperRight1, lowerLeft1, stencilColor);

        Pht::Vec2 lowerLeft2 {0.0f, cornerRadius};
        Pht::Vec2 upperRight2 {cornerRadius, menuSize.y - cornerRadius};
        rasterizer.DrawRectangle(upperRight2, lowerLeft2, stencilColor);

        Pht::Vec2 lowerLeft3 {menuSize.x - cornerRadius, cornerRadius};
        Pht::Vec2 upperRight3 {menuSize.x, menuSize.y - cornerRadius};
        rasterizer.DrawRectangle(upperRight3, lowerLeft3, stencilColor);

        rasterizer.EnableStencilTest();
    }

    void DrawStripes(Pht::OfflineRasterizer& rasterizer, const Pht::Vec2& menuSize) {
        const auto stripeWidth {(stripeStep / 2.0f) / std::sqrt(2.0f)};
        
        for (auto pos {0.0f}; pos < menuSize.x + menuSize.y; pos += stripeStep) {
            Pht::Vec2 trapezoidLowerLeft {0.0f, menuSize.y - pos};
            Pht::Vec2 trapezoidUpperRight {pos, menuSize.y};
            rasterizer.DrawTiltedTrapezoid315(trapezoidUpperRight,
                                              trapezoidLowerLeft,
                                              stripeWidth,
                                              whiteColor,
                                              Pht::DrawOver::Yes);
        }
    }
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
        case Size::Small:
            mSize = {sizeX, 11.5f};
            break;
    }
    
    Pht::IVec2 imageSize {
        static_cast<int>(mSize.x * xScaleFactor),
        static_cast<int>(mSize.y * yScaleFactor)
    };
    
    auto rasterizer {std::make_unique<Pht::OfflineRasterizer>(mSize, imageSize)};
    FillStencilBuffer(*rasterizer, mSize);
    
    rasterizer->DrawRectangle(mSize, {0.0f, 0.0f}, grayColor);
    DrawStripes(*rasterizer, mSize);
    
    auto image {rasterizer->ProduceImage()};
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    auto& sceneManager {engine.GetSceneManager()};
    mRenderableObject = sceneManager.CreateRenderableObject(Pht::QuadMesh {mSize.x, mSize.y},
                                                            imageMaterial);
}
