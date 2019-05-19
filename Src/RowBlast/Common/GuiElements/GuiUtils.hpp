#ifndef GuiUtils_hpp
#define GuiUtils_hpp

#include <memory>

// Game includes.
#include "MenuButton.hpp"
#include "MenuWindow.hpp"

namespace Pht {
    class IEngine;
    class GuiView;
    class SceneResources;
}

namespace RowBlast {
    class GuiResources;
    class CommonResources;
    
    namespace GuiUtils {
        std::unique_ptr<MenuButton> CreateCloseButton(Pht::IEngine& engine,
                                                      Pht::GuiView& view,
                                                      const GuiResources& guiResources,
                                                      PotentiallyZoomedScreen zoom);
        std::unique_ptr<MenuButton> CreatePlayOnButton(Pht::IEngine& engine,
                                                       Pht::GuiView& view,
                                                       const Pht::Vec3& position,
                                                       int priceInCoins,
                                                       const CommonResources& commonResources,
                                                       PotentiallyZoomedScreen zoom);
        std::unique_ptr<MenuButton> CreateMediumPlayButton(Pht::IEngine& engine,
                                                           Pht::GuiView& view,
                                                           const Pht::Vec3& position,
                                                           const GuiResources& guiResources,
                                                           PotentiallyZoomedScreen zoom);
        void CreateIcon(Pht::IEngine& engine,
                        Pht::GuiView& view,
                        const std::string& filename,
                        const Pht::Vec3& position,
                        const Pht::Vec2& size,
                        Pht::SceneObject& parent,
                        const Pht::Vec4& color = {0.95f, 0.95f, 0.95f, 1.0f});
        Pht::SceneObject& CreateIconWithShadow(Pht::IEngine& engine,
                                               Pht::SceneResources& sceneResources,
                                               const std::string& filename,
                                               const Pht::Vec3& position,
                                               const Pht::Vec2& size,
                                               Pht::SceneObject& parent,
                                               const Pht::Vec4& color,
                                               const Pht::Optional<Pht::Vec4>& shadowColor,
                                               const Pht::Optional<Pht::Vec3>& shadowOffset);
        void CreateTitleBarLine(Pht::IEngine& engine,
                                Pht::GuiView& view,
                                float titleBarHeight = 2.6f);
    }
}

#endif
