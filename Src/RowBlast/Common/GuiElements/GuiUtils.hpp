#ifndef GuiUtils_hpp
#define GuiUtils_hpp

#include <memory>

// Game includes.
#include "MenuButton.hpp"
#include "MenuWindow.hpp"

namespace Pht {
    class IEngine;
    class GuiView;
}

namespace RowBlast {
    class GuiResources;
    
    namespace GuiUtils {
        std::unique_ptr<MenuButton> CreateCloseButton(Pht::IEngine& engine,
                                                      Pht::GuiView& view,
                                                      const GuiResources& guiResources,
                                                      PotentiallyZoomedScreen zoom);
        void CreateIcon(Pht::IEngine& engine,
                        Pht::GuiView& view,
                        const std::string& filename,
                        const Pht::Vec3& position,
                        const Pht::Vec2& size,
                        Pht::SceneObject& parent);
        void CreateTitleBarLine(Pht::IEngine& engine, Pht::GuiView& view);
    }
}

#endif
