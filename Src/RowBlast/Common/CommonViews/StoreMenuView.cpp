#include "StoreMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

StoreMenuView::StoreMenuView(Pht::IEngine& engine,
                             const CommonResources& commonResources,
                             PotentiallyZoomedScreen zoom) {
    auto& guiResources {commonResources.GetGuiResources()};
    auto& menuWindow {guiResources.GetLargeDarkMenuWindow(zoom)};
    
    auto menuWindowSceneObject {std::make_unique<Pht::SceneObject>(&menuWindow.GetRenderable())};
    menuWindowSceneObject->GetTransform().SetPosition({0.0f, 0.0f, UiLayer::background});
    AddSceneObject(std::move(menuWindowSceneObject));

    auto windowSize {menuWindow.GetSize()};
    SetSize(windowSize);
    
    auto& largeTextProperties {guiResources.GetLargeWhiteTextProperties(zoom)};
    CreateText({-1.2f, 8.25f, UiLayer::text}, "SHOP", largeTextProperties);
    
    Pht::Vec3 closeButtonPosition {
        GetSize().x / 2.0f - 1.3f,
        GetSize().y / 2.0f - 1.3f,
        UiLayer::textRectangle
    };

    Pht::Vec2 closeButtonInputSize {55.0f, 55.0f};

    MenuButton::Style closeButtonStyle;
    closeButtonStyle.mPressedScale = 1.05f;
    closeButtonStyle.mRenderableObject = &guiResources.GetCloseButton(zoom);

    mCloseButton = std::make_unique<MenuButton>(engine,
                                                *this,
                                                closeButtonPosition,
                                                closeButtonInputSize,
                                                closeButtonStyle);

    auto& coin {
        CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                          commonResources.GetMaterials().GetGoldMaterial(),
                          engine.GetSceneManager())
    };
    auto& coinTransform {coin.GetTransform()};
    coinTransform.SetPosition({-GetSize().x / 2.0f + 0.8f, 8.55f, UiLayer::root});
    coinTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinTransform.SetScale(0.7f);
    GetRoot().AddChild(coin);
    
    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize20(PotentiallyZoomedScreen::No),
        1.2f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };
    CreateText({-GetSize().x / 2.0f + 1.3f, 8.3f, UiLayer::text}, "45", textProperties);
    
    Pht::Material lineMaterial {Pht::Color{0.6f, 0.8f, 1.0f}};
    lineMaterial.SetOpacity(0.3f);
    auto& sceneManager {engine.GetSceneManager()};
    auto& lineSceneObject {
        CreateSceneObject(Pht::QuadMesh {GetSize().x - 0.6f, 0.06f}, lineMaterial, sceneManager)
    };
    lineSceneObject.GetTransform().SetPosition({0.0f, GetSize().y / 2.0f - 2.6f, UiLayer::textRectangle});
    GetRoot().AddChild(lineSceneObject);
    
    auto productXPosition {windowSize.x / 4.0f - 0.125f};
    
    CreateProduct({-productXPosition, 4.45f, UiLayer::root}, "10", "20,00 kr", engine, commonResources, zoom);
    CreateProduct({productXPosition, 4.45f, UiLayer::root}, "50", "85,00 kr", engine, commonResources, zoom);
    CreateProduct({-productXPosition, -1.25f, UiLayer::root}, "100", "159,00 kr", engine, commonResources, zoom);
    CreateProduct({productXPosition, -1.25f, UiLayer::root}, "250", "319,00 kr", engine, commonResources, zoom);
    CreateProduct({-productXPosition, -6.95f, UiLayer::root}, "500", "595,00 kr", engine, commonResources, zoom);
}

void StoreMenuView::CreateProduct(const Pht::Vec3& position,
                                  const std::string& numCoins,
                                  const std::string& price,
                                  Pht::IEngine& engine,
                                  const CommonResources& commonResources,
                                  PotentiallyZoomedScreen zoom) {
    auto& guiResources {commonResources.GetGuiResources()};
    
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition(position);
    GetRoot().AddChild(container);
    
    auto productWidth {GetSize().x / 2.0f - 0.35f};
    auto productHeight {5.6f};
    auto& sceneManager {engine.GetSceneManager()};
/*
    Pht::Material panelMaterial {Pht::Color{0.2f, 0.3f, 0.5f}};
    panelMaterial.SetOpacity(0.3f);
    auto& panelSceneObject {
        CreateSceneObject(Pht::QuadMesh {productWidth, productHeight}, panelMaterial, sceneManager)
    };
    panelSceneObject.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::panel});
    container.AddChild(panelSceneObject);
*/

    Pht::QuadMesh::Vertices panelVertices  {
        {{-productWidth / 2.0f, -productHeight / 2.0f, 0.0f}, {0.3f, 0.5f, 0.8f, 0.3f}},
        {{productWidth / 2.0f, -productHeight / 2.0f, 0.0f}, {0.3f, 0.5f, 0.8f, 0.3f}},
        {{productWidth / 2.0f, productHeight / 2.0f, 0.0f}, {0.1f, 0.2f, 0.4f, 0.0f}},
        {{-productWidth / 2.0f, productHeight / 2.0f, 0.0f}, {0.1f, 0.2f, 0.4f, 0.0f}}
    };

    Pht::Material panelMaterial;
    panelMaterial.SetBlend(Pht::Blend::Yes);
    auto& panelSceneObject {
        CreateSceneObject(Pht::QuadMesh {panelVertices}, panelMaterial, sceneManager)
    };
    panelSceneObject.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::panel});
    container.AddChild(panelSceneObject);

    auto& coin {
        CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                          commonResources.GetMaterials().GetGoldMaterial(),
                          sceneManager)
    };
    auto& coinTransform {coin.GetTransform()};
    coinTransform.SetPosition({-0.6f, -0.7f, UiLayer::root});
    coinTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinTransform.SetScale(0.7f);
    container.AddChild(coin);

    MenuButton::Style buttonStyle;
    buttonStyle.mPressedScale = 1.05f;
    buttonStyle.mTextScale = 1.2f;
    buttonStyle.mRenderableObject = &guiResources.GetThinGreenGlossyButton(zoom);
    buttonStyle.mSelectedRenderableObject = &guiResources.GetThinDarkGreenGlossyButton(zoom);

    Pht::Vec2 buttonInputSize {78.0f, 43.0f};
    Pht::Vec3 buttonPosition {0.0f, -2.05f, UiLayer::textRectangle};
    
    auto button {
        std::make_unique<MenuButton>(engine,
                                     *this,
                                     container,
                                     buttonPosition,
                                     buttonInputSize,
                                     buttonStyle)
    };
    
    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize20(PotentiallyZoomedScreen::No),
        1.2f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };
    
    button->CreateText({-1.4f, -0.21f, UiLayer::buttonText}, price, textProperties);
    mProductButtons.push_back(std::move(button));
    
    CreateText({-0.1f, -0.95f, UiLayer::text}, numCoins, textProperties, container);
}
