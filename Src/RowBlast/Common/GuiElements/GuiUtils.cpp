#include "GuiUtils.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "GuiView.hpp"
#include "Material.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "GuiResources.hpp"
#include "UiLayer.hpp"
#include "CommonResources.hpp"

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

std::unique_ptr<MenuButton> GuiUtils::CreatePlayOnButton(Pht::IEngine& engine,
                                                         Pht::GuiView& view,
                                                         const Pht::Vec3& position,
                                                         int priceInCoins,
                                                         const CommonResources& commonResources,
                                                         PotentiallyZoomedScreen zoom) {
    auto& guiResources = commonResources.GetGuiResources();
    Pht::Vec2 playOnButtonInputSize {194.0f, 50.0f};

    MenuButton::Style playOnButtonStyle;
    playOnButtonStyle.mPressedScale = 1.05f;
    playOnButtonStyle.mTextScale = 1.05f;
    playOnButtonStyle.mRenderableObject = &guiResources.GetLargeBlueGlossyButton(zoom);
    playOnButtonStyle.mSelectedRenderableObject = &guiResources.GetLargeDarkBlueGlossyButton(zoom);

    auto playOnButton = std::make_unique<MenuButton>(engine,
                                                     view,
                                                     position,
                                                     playOnButtonInputSize,
                                                     playOnButtonStyle);
    playOnButton->CreateIcon("play.png",
                             {-3.3f, 0.0f, UiLayer::buttonText},
                             {0.7f, 0.7f},
                             {1.0f, 1.0f, 1.0f, 1.0f},
                             Pht::Vec4 {0.2f, 0.2f, 0.2f, 0.5f},
                             Pht::Vec3 {-0.05f, -0.05f, UiLayer::textShadow});

    Pht::TextProperties buttonTextProperties {
        commonResources.GetHussarFontSize27(zoom),
        1.05f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };

    playOnButton->CreateText({-2.7f, -0.24f, UiLayer::buttonText},
                             "CONTINUE           " + std::to_string(priceInCoins),
                             buttonTextProperties);

    auto& coin =
        view.CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                               commonResources.GetMaterials().GetGoldMaterial(),
                               engine.GetSceneManager());
    auto& coinTransform = coin.GetTransform();
    coinTransform.SetPosition({2.25f, 0.0f, UiLayer::buttonOverlayObject2});
    coinTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinTransform.SetScale(0.9f);
    playOnButton->GetSceneObject().AddChild(coin);

    Pht::Material shaddowMaterial {Pht::Color{0.05f, 0.05f, 0.05f}};
    shaddowMaterial.SetOpacity(0.14f);
    shaddowMaterial.SetDepthTestAllowedOverride(true);
    auto& coinShaddow =
        view.CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                               shaddowMaterial,
                               engine.GetSceneManager());
    auto& coinShaddowTransform = coinShaddow.GetTransform();
    coinShaddowTransform.SetPosition({2.12f, -0.12f, UiLayer::buttonOverlayObject1});
    coinShaddowTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinShaddowTransform.SetScale(0.9f);
    playOnButton->GetSceneObject().AddChild(coinShaddow);
    
    return playOnButton;
}

std::unique_ptr<MenuButton> GuiUtils::CreateMediumPlayButton(Pht::IEngine& engine,
                                                             Pht::GuiView& view,
                                                             const Pht::Vec3& position,
                                                             const GuiResources& guiResources,
                                                             PotentiallyZoomedScreen zoom) {
    Pht::Vec2 playButtonInputSize {194.0f, 43.0f};

    MenuButton::Style playButtonStyle;
    playButtonStyle.mRenderableObject = &guiResources.GetMediumBlueGlossyButton(zoom);
    playButtonStyle.mSelectedRenderableObject = &guiResources.GetMediumDarkBlueGlossyButton(zoom);
    playButtonStyle.mPressedScale = 1.05f;

    auto playButton = std::make_unique<MenuButton>(engine,
                                                   view,
                                                   position,
                                                   playButtonInputSize,
                                                   playButtonStyle);
    playButton->CreateIcon("play.png",
                           {-1.15f, 0.03f, UiLayer::buttonText},
                           {0.65f, 0.65f},
                           {1.0f, 1.0f, 1.0f, 1.0f},
                           Pht::Optional<Pht::Vec4> {},
                           Pht::Optional<Pht::Vec3> {});
    playButton->CreateText({-0.6f, -0.23f, UiLayer::buttonText},
                           "PLAY",
                           guiResources.GetWhiteButtonTextProperties(zoom));
    
    return playButton;
}

Pht::SceneObject& GuiUtils::CreateIcon(Pht::IEngine& engine,
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
    
    return iconSceneObject;
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
