#include "CloseButton.hpp"

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
    constexpr auto circleRadius {0.87f};
    constexpr auto width {0.072f};
    constexpr auto padding {0.01f};
    constexpr auto crossSize {0.55f};
    
    void DrawButton(Pht::OfflineRasterizer& rasterizer) {
        Pht::Vec4 fillColor {0.0f, 0.385f, 1.0f, 0.6f};
        Pht::Vec4 whiteColor {1.0f, 1.0f, 1.0f, 0.8f};

        rasterizer.DrawCircle({circleRadius, circleRadius},
                              circleRadius - padding,
                              circleRadius - padding,
                              fillColor);
        rasterizer.DrawCircle({circleRadius, circleRadius},
                              circleRadius - width * 2.0f,
                              width,
                              whiteColor,
                              Pht::DrawOver::Yes);

        auto quarterWidth {width / 4.0f};
        
        Pht::Vec2 lowerLeft {
            circleRadius - crossSize / 2.0f + quarterWidth,
            circleRadius - crossSize / 2.0f
        };
        
        Pht::Vec2 upperRight {
            circleRadius + crossSize / 2.0f + quarterWidth,
            circleRadius + crossSize / 2.0f
        };

        rasterizer.DrawTiltedTrapezoid315(upperRight,
                                          lowerLeft,
                                          width,
                                          whiteColor,
                                          Pht::DrawOver::Yes);

        Pht::Vec2 upperLeft {
            circleRadius - crossSize / 2.0f - quarterWidth,
            circleRadius + crossSize / 2.0f
        };

        Pht::Vec2 lowerRight {
            circleRadius + crossSize / 2.0f -quarterWidth,
            circleRadius - crossSize / 2.0f
        };
    
        rasterizer.DrawTiltedTrapezoid225(upperLeft,
                                          lowerRight,
                                          width,
                                          whiteColor,
                                          Pht::DrawOver::Yes);
    }
}

std::unique_ptr<Pht::RenderableObject>
RowBlast::CreateCloseButton(Pht::IEngine& engine,
                            const CommonResources& commonResources,
                            PotentiallyZoomedScreen potentiallyZoomed) {
    auto& renderer {engine.GetRenderer()};
    auto& renderBufferSize {renderer.GetRenderBufferSize()};
    
    auto& frustumSize {
        potentiallyZoomed == PotentiallyZoomedScreen::Yes ?
        commonResources.GetHudFrustumSizePotentiallyZoomedScreen() : renderer.GetHudFrustumSize()
    };
    
    auto xScaleFactor {static_cast<float>(renderBufferSize.x) / static_cast<float>(frustumSize.x)};
    auto yScaleFactor {static_cast<float>(renderBufferSize.y) / static_cast<float>(frustumSize.y)};

    Pht::Vec2 coordinateSystemSize {circleRadius * 2.0f, circleRadius * 2.0f};

    Pht::IVec2 imageSize {
        static_cast<int>(coordinateSystemSize.x * xScaleFactor) * 3,
        static_cast<int>(coordinateSystemSize.y * yScaleFactor) * 3
    };
    
    auto rasterizer {std::make_unique<Pht::OfflineRasterizer>(coordinateSystemSize, imageSize)};

    DrawButton(*rasterizer);

    auto image {rasterizer->ProduceImage()};
    Pht::Material imageMaterial {*image, Pht::GenerateMipmap::Yes};
    imageMaterial.SetBlend(Pht::Blend::Yes);
    auto& sceneManager {engine.GetSceneManager()};
    return sceneManager.CreateRenderableObject(Pht::QuadMesh {coordinateSystemSize.x, coordinateSystemSize.y},
                                               imageMaterial);
}
