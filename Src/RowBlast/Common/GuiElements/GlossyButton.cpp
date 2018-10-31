#include "GlossyButton.hpp"

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
    
    struct ButtonColors {
        Pht::Vec4 mShadeColor;
        Pht::Vec4 mGlossColor;
        Pht::Vec4 mUpperGradientColor;
        Pht::Vec4 mLowerGradientColor;
    };
    
    const ButtonColors blueButtonColors {
        .mShadeColor = {0.0f, 0.17f, 0.34f, 1.0f},
        .mGlossColor = {0.11f, 0.66f, 1.0f, 1.0f},
        .mUpperGradientColor = {0.0f, 0.585f, 0.935f, 1.0f},
        .mLowerGradientColor = {0.0f, 0.285f, 0.585f, 1.0f}
    };

    const ButtonColors darkBlueButtonColors {
        .mShadeColor = {0.0f, 0.15f, 0.3f, 1.0f},
        .mGlossColor = {0.08f, 0.55f, 0.83f, 1.0f},
        .mUpperGradientColor = {0.0f, 0.45f, 0.7f, 1.0f},
        .mLowerGradientColor = {0.0f, 0.225f, 0.5f, 1.0f}
    };

    const ButtonColors grayButtonColors {
        .mShadeColor = {0.15f, 0.15f, 0.15f, 1.0f},
        .mGlossColor = {0.6f, 0.6f, 0.6f, 1.0f},
        .mUpperGradientColor = {0.52f, 0.52f, 0.52f, 1.0f},
        .mLowerGradientColor = {0.28f, 0.28f, 0.28f, 1.0f}
    };

    Pht::Vec2 GetSize(ButtonSize size) {
        switch (size) {
            case ButtonSize::Large:
                return {9.4f, 2.6f};
            case ButtonSize::Medium:
                return {9.0f, 2.1f};
            case ButtonSize::Small:
                return {3.6f, 2.1f};
        }
    }
    
    float GetCornerRadius(ButtonSize size) {
        switch (size) {
            case ButtonSize::Large:
                return 0.35f;
            case ButtonSize::Medium:
            case ButtonSize::Small:
                return 0.3f;
        }
    }
    
    float GetShadedAreaHeight(ButtonSize size) {
        switch (size) {
            case ButtonSize::Large:
                return 0.1f;
            case ButtonSize::Medium:
            case ButtonSize::Small:
                return 0.08f;
        }
    }

    float GetGlossyAreaHeight(ButtonSize size) {
        switch (size) {
            case ButtonSize::Large:
                return 1.1f;
            case ButtonSize::Medium:
            case ButtonSize::Small:
                return 0.89f;
        }
    }
    
    const ButtonColors& GetButtonColors(ButtonColor buttonColor) {
        switch (buttonColor) {
            case ButtonColor::Blue:
                return blueButtonColors;
            case ButtonColor::Green:
                return grayButtonColors;
            case ButtonColor::Gray:
                return grayButtonColors;
            case ButtonColor::DarkBlue:
                return darkBlueButtonColors;
            case ButtonColor::DarkGreen:
                return grayButtonColors;
        }
    }

    void FillStencilBuffer(Pht::OfflineRasterizer& rasterizer,
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

        Pht::Vec2 lowerLeft2 {xPadding, cornerRadius + lowerYPadding};
        Pht::Vec2 upperRight2 {cornerRadius + xPadding, size.y - cornerRadius - upperYPadding};
        rasterizer.DrawRectangle(upperRight2, lowerLeft2, stencilColor);

        Pht::Vec2 lowerLeft3 {size.x - cornerRadius - xPadding, cornerRadius + lowerYPadding};
        Pht::Vec2 upperRight3 {size.x - xPadding, size.y - cornerRadius - upperYPadding};
        rasterizer.DrawRectangle(upperRight3, lowerLeft3, stencilColor);

        rasterizer.EnableStencilTest();
    }
    
    void DrawShadedArea(Pht::OfflineRasterizer& rasterizer,
                        ButtonSize buttonSize,
                        const ButtonColors& buttonColors) {
        auto size {GetSize(buttonSize)};
        
        FillStencilBuffer(rasterizer,
                          size,
                          GetCornerRadius(buttonSize),
                          0.0f,
                          0.0f,
                          0.0f);

        Pht::Vec2 lowerLeft {0.0f, 0.0f};
        Pht::Vec2 upperRight {size.x, size.y};
        rasterizer.DrawRectangle(upperRight, lowerLeft, buttonColors.mShadeColor, Pht::DrawOver::Yes);
    }
    
    void DrawGlossyArea(Pht::OfflineRasterizer& rasterizer,
                        ButtonSize buttonSize,
                        const ButtonColors& buttonColors) {
        auto size {GetSize(buttonSize)};
        
        FillStencilBuffer(rasterizer,
                          size,
                          GetCornerRadius(buttonSize),
                          0.0f,
                          0.0f,
                          GetShadedAreaHeight(buttonSize));
    
        Pht::Vec2 lowerLeft {0.0f, 0.0f};
        Pht::Vec2 upperRight {size.x, size.y};
        rasterizer.DrawRectangle(upperRight, lowerLeft, buttonColors.mGlossColor, Pht::DrawOver::Yes);
    }

    void DrawGradientArea(Pht::OfflineRasterizer& rasterizer,
                          ButtonSize buttonSize,
                          const ButtonColors& buttonColors) {
        auto size {GetSize(buttonSize)};
        auto shadedAreaHeight {GetShadedAreaHeight(buttonSize)};
        auto glossyAreaHeight {GetGlossyAreaHeight(buttonSize)};
        
        FillStencilBuffer(rasterizer,
                          size,
                          GetCornerRadius(buttonSize),
                          0.0f,
                          glossyAreaHeight,
                          shadedAreaHeight);

        Pht::OfflineRasterizer::VerticalGradientColors gradientColors {
            buttonColors.mLowerGradientColor,
            buttonColors.mUpperGradientColor
        };
        Pht::Vec2 lowerLeft {0.0f, shadedAreaHeight};
        Pht::Vec2 upperRight {size.x, size.y - glossyAreaHeight};
        rasterizer.DrawGradientRectangle(upperRight, lowerLeft, gradientColors, Pht::DrawOver::Yes);
    }

    void DrawButton(Pht::OfflineRasterizer& rasterizer,
                    ButtonSize buttonSize,
                    ButtonColor buttonColor) {
        auto buttonColors {GetButtonColors(buttonColor)};
        
        DrawShadedArea(rasterizer, buttonSize, buttonColors);
        DrawGlossyArea(rasterizer, buttonSize, buttonColors);
        DrawGradientArea(rasterizer, buttonSize, buttonColors);
    }
}

std::unique_ptr<Pht::RenderableObject>
RowBlast::CreateGlossyButton(Pht::IEngine& engine,
                             const CommonResources& commonResources,
                             ButtonSize size,
                             ButtonColor color,
                             PotentiallyZoomedScreen potentiallyZoomed) {
    auto& renderer {engine.GetRenderer()};
    auto& renderBufferSize {renderer.GetRenderBufferSize()};
    
    auto& frustumSize {
        potentiallyZoomed == PotentiallyZoomedScreen::Yes ?
        commonResources.GetHudFrustumSizePotentiallyZoomedScreen() : renderer.GetHudFrustumSize()
    };
    
    auto xScaleFactor {static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)};
    auto yScaleFactor {static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)};
    auto coordinateSystemSize {GetSize(size)};

    Pht::IVec2 imageSize {
        static_cast<int>(coordinateSystemSize.x * xScaleFactor),
        static_cast<int>(coordinateSystemSize.y * yScaleFactor)
    };
    
    auto rasterizer {std::make_unique<Pht::OfflineRasterizer>(coordinateSystemSize, imageSize)};

    DrawButton(*rasterizer, size, color);

    auto image {rasterizer->ProduceImage()};
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    auto& sceneManager {engine.GetSceneManager()};
    return sceneManager.CreateRenderableObject(Pht::QuadMesh {coordinateSystemSize.x, coordinateSystemSize.y},
                                               imageMaterial);
}
