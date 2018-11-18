#include "StoreMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"
#include "IParticleSystem.hpp"
#include "ParticleEffect.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto animationDuration {6.0f};
    constexpr auto rotationAmplitude {17.5f};
    constexpr auto coinPilesXAngle {30.0f};
}

StoreMenuView::StoreMenuView(Pht::IEngine& engine,
                             const CommonResources& commonResources,
                             PotentiallyZoomedScreen zoom) :
    mEngine {engine} {

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


    Product product;
    CreateGlowEffect(engine, container, product);
    CreateTwinklesEffect(engine, container, product);
    
    auto& coinPile {
        CreateSceneObject(Pht::ObjMesh {"coin_pile_4120.obj", 40.0f},
                          commonResources.GetMaterials().GetGoldMaterial(),
                          sceneManager)
    };
    product.mCoinPile = &coinPile;
    auto& coinPileTransform {coinPile.GetTransform()};
    coinPileTransform.SetPosition({0.0f, 0.7f, UiLayer::textRectangle});
    coinPileTransform.SetRotation({coinPilesXAngle, 0.0f, 0.0f});
    coinPileTransform.SetScale(1.0f);
    container.AddChild(coinPile);



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
    product.mButton = std::move(button);
    
    CreateText({-0.1f, -0.95f, UiLayer::text}, numCoins, textProperties, container);
    
    mProducts.push_back(std::move(product));
}

void StoreMenuView::CreateGlowEffect(Pht::IEngine& engine,
                                     Pht::SceneObject& parentObject,
                                     Product& product) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mTimeToLive = 0.0f,
        .mFrequency = 0.0f,
        .mBurst = 2
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{0.46f, 0.2f, 0.65f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare24.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.5f,
        .mZAngularVelocity = 20.0f,
        .mZAngularVelocityRandomPart = 10.0f,
        .mSize = Pht::Vec2{11.25f, 11.25f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.0f,
        .mGrowDuration = 0.5f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    product.mGlowEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                         particleEmitterSettings,
                                                                         Pht::RenderMode::Triangles);
    auto& material {product.mGlowEffect->GetRenderable()->GetMaterial()};
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
    
    product.mGlowEffect->GetTransform().SetPosition({0.0f, 0.7f, -1.9f});
    parentObject.AddChild(*product.mGlowEffect);
}

void StoreMenuView::CreateTwinklesEffect(Pht::IEngine& engine,
                                         Pht::SceneObject& parentObject,
                                         Product& product) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = Pht::Vec3{1.5f, 1.5f, 0.0f},
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mFrequency = 2.0f
    };
    
    Pht::ParticleSettings particleSettings {
        .mVelocity = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mVelocityRandomPart = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mColor = Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.1f, 0.1f, 0.1f, 0.0f},
        .mTextureFilename = "particle_sprite_twinkle_blurred.png",
        .mTimeToLive = 0.7f,
        .mTimeToLiveRandomPart = 0.4f,
        .mFadeOutDuration = 0.0f,
        .mZAngularVelocityRandomPart = 400.0f,
        .mSize = Pht::Vec2{2.42f, 2.42f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.4f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    product.mTwinklesEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                             particleEmitterSettings,
                                                                             Pht::RenderMode::Triangles);
    product.mTwinklesEffect->GetTransform().SetPosition({0.0f, 0.7f, UiLayer::root});
    parentObject.AddChild(*product.mTwinklesEffect);
}

void StoreMenuView::Init() {
    mAnimationTime = 0.0f;
    
    for (auto& product: mProducts) {
        product.mGlowEffect->GetComponent<Pht::ParticleEffect>()->Stop();
        product.mTwinklesEffect->GetComponent<Pht::ParticleEffect>()->Stop();
    }
}

void StoreMenuView::StartEffects() {
    for (auto& product: mProducts) {
        product.mGlowEffect->GetComponent<Pht::ParticleEffect>()->Start();
        product.mTwinklesEffect->GetComponent<Pht::ParticleEffect>()->Start();
    }
}

void StoreMenuView::Update() {
    auto dt {mEngine.GetLastFrameSeconds()};
    mAnimationTime += dt;
    
    if (mAnimationTime > animationDuration) {
        mAnimationTime = 0.0f;
    }
    
    for (auto& product: mProducts) {
        auto t {mAnimationTime * 2.0f * 3.1415f / animationDuration};
        auto yAngle {rotationAmplitude * sin(t)};
        
        product.mCoinPile->GetTransform().SetRotation({coinPilesXAngle, yAngle, 0.0f});

        product.mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
        product.mTwinklesEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    }
}
