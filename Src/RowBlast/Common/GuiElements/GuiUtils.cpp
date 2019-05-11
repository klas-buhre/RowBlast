#include "GuiUtils.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "GuiView.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "GuiResources.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

std::unique_ptr<MenuButton> GuiUtils::CreateCloseButton(Pht::IEngine& engine,
                                                        Pht::GuiView& view,
                                                        const GuiResources& guiResources,
                                                        PotentiallyZoomedScreen zoom) {
    Pht::Vec3 closeButtonPosition {
        view.GetSize().x / 2.0f - 1.3f,
        view.GetSize().y / 2.0f - 1.3f,
        UiLayer::textRectangle
    };

    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};

    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);
    
    return std::make_unique<MenuButton>(engine,
                                        view,
                                        closeButtonPosition,
                                        closeButtonInputSize,
                                        closeButtonStyle);
}

void GuiUtils::CreateIcon(Pht::IEngine& engine,
                          Pht::GuiView& view,
                          const std::string& filename,
                          const Pht::Vec3& position,
                          const Pht::Vec2& size,
                          Pht::SceneObject& parent,
                          const Pht::Vec4& color) {
    Pht::Material iconMaterial {filename, 0.0f, 0.0f, 0.0f, 0.0f};
    iconMaterial.SetBlend(Pht::Blend::Yes);
    iconMaterial.SetOpacity(color.w);
    iconMaterial.SetAmbient(Pht::Color{color.x, color.y, color.z});
    
    auto& sceneManager = engine.GetSceneManager();
    auto& iconSceneObject =
        view.CreateSceneObject(Pht::QuadMesh {size.x, size.y}, iconMaterial, sceneManager);
    
    iconSceneObject.GetTransform().SetPosition(position);
    parent.AddChild(iconSceneObject);
}

Pht::SceneObject& GuiUtils::CreateIconWithShadow(Pht::IEngine& engine,
                                                 Pht::SceneResources& sceneResources,
                                                 const std::string& filename,
                                                 const Pht::Vec3& position,
                                                 const Pht::Vec2& size,
                                                 Pht::SceneObject& parent,
                                                 const Pht::Vec4& color,
                                                 const Pht::Optional<Pht::Vec4>& shadowColor,
                                                 const Pht::Optional<Pht::Vec3>& shadowOffset) {
    auto containerSceneObject = std::make_unique<Pht::SceneObject>();
    auto& container = *containerSceneObject;
    containerSceneObject->GetTransform().SetPosition(position);
    parent.AddChild(*containerSceneObject);
    sceneResources.AddSceneObject(std::move(containerSceneObject));
    
    auto& sceneManager = engine.GetSceneManager();

    Pht::Material iconMaterial {filename, 0.0f, 0.0f, 0.0f, 0.0f};
    iconMaterial.SetBlend(Pht::Blend::Yes);
    iconMaterial.SetOpacity(color.w);
    iconMaterial.SetAmbient(Pht::Color{color.x, color.y, color.z});
    
    auto iconSceneObject =
        sceneManager.CreateSceneObject(Pht::QuadMesh {size.x, size.y},
                                       iconMaterial,
                                       sceneResources);
    
    container.AddChild(*iconSceneObject);
    sceneResources.AddSceneObject(std::move(iconSceneObject));

    if (shadowColor.HasValue()) {
        auto& shadowColorValue = shadowColor.GetValue();
        Pht::Material shadowMaterial {filename, 0.0f, 0.0f, 0.0f, 0.0f};
        shadowMaterial.SetBlend(Pht::Blend::Yes);
        shadowMaterial.SetOpacity(shadowColorValue.w);
        shadowMaterial.SetAmbient(Pht::Color{shadowColorValue.x, shadowColorValue.y, shadowColorValue.z});
        
        auto shadowSceneObject =
            sceneManager.CreateSceneObject(Pht::QuadMesh {size.x, size.y},
                                           shadowMaterial,
                                           sceneResources);
        
        shadowSceneObject->GetTransform().SetPosition(shadowOffset.GetValue());
        container.AddChild(*shadowSceneObject);
        sceneResources.AddSceneObject(std::move(shadowSceneObject));
    }
    
    return container;
}

void GuiUtils::CreateTitleBarLine(Pht::IEngine& engine, Pht::GuiView& view, float titleBarHeight) {
    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    
    auto& sceneManager = engine.GetSceneManager();
    auto& lineSceneObject =
        view.CreateSceneObject(Pht::QuadMesh {view.GetSize().x - 1.5f, 0.06f},
                               lineMaterial,
                               sceneManager);
    
    Pht::Vec3 position {0.0f, view.GetSize().y / 2.0f - titleBarHeight, UiLayer::textRectangle};
    lineSceneObject.GetTransform().SetPosition(position);
    view.GetRoot().AddChild(lineSceneObject);
}
