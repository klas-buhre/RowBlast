#include "StoreMenuView.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "ISceneManager.hpp"
#include "TextComponent.hpp"
#include "QuadMesh.hpp"
#include "ObjMesh.hpp"
#include "CylinderMesh.hpp"
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
    
    CreateRenderables(engine, commonResources);

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

    auto& coin {CreateSceneObject()};
    coin.SetRenderable(mCoinRenderable.get());
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
    
    CreateProduct({-productXPosition, 4.45f, UiLayer::root},
                  1.15f,
                  10,
                  "20,00 kr",
                  {0.75f, 0.75f, 0.0f},
                  {9.0f, 9.0f},
                  engine,
                  commonResources,
                  zoom);
    CreateProduct({productXPosition, 4.45f, UiLayer::root},
                  1.0f, 50,
                  "85,00 kr",
                  {1.5f, 1.25f, 0.0f},
                  {11.25f, 11.25f},
                  engine,
                  commonResources,
                  zoom);
    CreateProduct({-productXPosition, -1.25f, UiLayer::root},
                  1.0f,
                  100,
                  "159,00 kr",
                  {1.5f, 1.25f, 0.0f},
                  {11.25f, 11.25f},
                  engine,
                  commonResources,
                  zoom);
    CreateProduct({productXPosition, -1.25f, UiLayer::root},
                  1.0f,
                  250,
                  "319,00 kr",
                  {1.5f, 1.25f, 0.0f},
                  {11.25f, 11.25f},
                  engine,
                  commonResources,
                  zoom);
    CreateProduct({-productXPosition, -6.95f, UiLayer::root},
                  1.0f,
                  500,
                  "595,00 kr",
                  {1.5f, 1.25f, 0.0f},
                  {12.0f, 12.0f},
                  engine,
                  commonResources,
                  zoom);
}

void StoreMenuView::CreateRenderables(Pht::IEngine& engine,
                                      const CommonResources& commonResources) {
    auto& sceneManager {engine.GetSceneManager()};
    auto& goldMaterial {commonResources.GetMaterials().GetGoldMaterial()};
    auto& lightGoldMaterial {commonResources.GetMaterials().GetLightGoldMaterial()};
    auto& lighterGoldMaterial {commonResources.GetMaterials().GetLighterGoldMaterial()};
    
    mCoinRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                                                          goldMaterial);
    mLightCoinRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                                                               lighterGoldMaterial);
    mShortCoinStackRenderable = sceneManager.CreateRenderableObject(Pht::CylinderMesh {0.4f, 0.5f, true},
                                                                    lighterGoldMaterial);
    mTallCoinStackRenderable = sceneManager.CreateRenderableObject(Pht::CylinderMesh {0.4f, 1.0f, true},
                                                                   lighterGoldMaterial);
    mTallerCoinStackRenderable = sceneManager.CreateRenderableObject(Pht::CylinderMesh {0.21f, 1.5f, true},
                                                                     lighterGoldMaterial);
    mCoinPileRenderable = sceneManager.CreateRenderableObject(Pht::ObjMesh {"coin_pile_4120.obj", 40.0f},
                                                              lightGoldMaterial);
}

void StoreMenuView::CreateProduct(const Pht::Vec3& position,
                                  float scale,
                                  int numCoins,
                                  const std::string& price,
                                  const Pht::Vec3& twinklesVolume,
                                  const Pht::Vec2& glowSize,
                                  Pht::IEngine& engine,
                                  const CommonResources& commonResources,
                                  PotentiallyZoomedScreen zoom) {
    auto& guiResources {commonResources.GetGuiResources()};
    
    auto& container {CreateSceneObject()};
    container.GetTransform().SetPosition(position);
    GetRoot().AddChild(container);
    
    auto productWidth {GetSize().x / 2.0f - 0.19f};
    auto productHeight {5.6f};
    auto& sceneManager {engine.GetSceneManager()};

    Pht::QuadMesh::Vertices panelVertices  {
        {{-productWidth / 2.0f, -productHeight / 2.0f, 0.0f}, {0.5f, 0.5f, 0.6f, 0.3f}},
        {{productWidth / 2.0f, -productHeight / 2.0f, 0.0f}, {0.5f, 0.5f, 0.6f, 0.3f}},
        {{productWidth / 2.0f, productHeight / 2.0f, 0.0f}, {0.1f, 0.1f, 0.1f, 0.0f}},
        {{-productWidth / 2.0f, productHeight / 2.0f, 0.0f}, {0.1f, 0.1f, 0.1f, 0.0f}}
    };

    Pht::Material panelMaterial;
    panelMaterial.SetBlend(Pht::Blend::Yes);
    auto& panelSceneObject {
        CreateSceneObject(Pht::QuadMesh {panelVertices}, panelMaterial, sceneManager)
    };
    panelSceneObject.GetTransform().SetPosition({0.0f, 0.0f, UiLayer::panel});
    container.AddChild(panelSceneObject);

    Product product;
    CreateGlowEffect(engine, container, product, glowSize);
    CreateTwinklesEffect(engine, container, product, twinklesVolume);
    CreateCoinPilesAndStacks(container, product, numCoins, scale);

    auto& coin {CreateSceneObject()};
    coin.SetRenderable(mCoinRenderable.get());
    auto& coinTransform {coin.GetTransform()};
    coinTransform.SetPosition({-0.6f, -0.7f, UiLayer::root});
    coinTransform.SetRotation({0.0f, 40.0f, 0.0f});
    coinTransform.SetScale(0.7f);
    container.AddChild(coin);

    MenuButton::Style buttonStyle;
    buttonStyle.mPressedScale = 1.05f;
    buttonStyle.mTextScale = 1.2f;
    buttonStyle.mRenderableObject = &guiResources.GetThinBlueGlossyButton(zoom);
    buttonStyle.mSelectedRenderableObject = &guiResources.GetThinDarkBlueGlossyButton(zoom);

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
    
    CreateText({-0.1f, -0.95f, UiLayer::text}, std::to_string(numCoins), textProperties, container);
    
    mProducts.push_back(std::move(product));
}

void StoreMenuView::CreateCoinPilesAndStacks(Pht::SceneObject& parentObject,
                                             Product& product,
                                             int numCoins,
                                             float scale) {
    auto& container {CreateSceneObject()};
    auto& transform {container.GetTransform()};
    transform.SetPosition({0.0f, 0.7f, UiLayer::textRectangle});
    transform.SetRotation({coinPilesXAngle, 0.0f, 0.0f});
    transform.SetScale(scale);
    parentObject.AddChild(container);
    
    product.mCoinPilesAndStacks = &container;

    switch (numCoins) {
        case 10:
            CreateCoinsFor10CoinsProduct(container);
            break;
        case 50:
            CreateCoinsFor50CoinsProduct(container);
            break;
        case 100:
            CreateCoinsFor100CoinsProduct(container);
            break;
        case 250:
            CreateCoinsFor250CoinsProduct(container);
            break;
        case 500:
            CreateCoinsFor500CoinsProduct(container);
            break;
        default:
            assert(false);
            break;
    }
}

void StoreMenuView::CreateCoinsFor10CoinsProduct(Pht::SceneObject& parentObject) {
    CreateCoin({-0.35f, 0.0f, 0.38f}, parentObject);
    CreateCoin({-0.25f, 0.13f, 0.38f}, parentObject);
    CreateShortCoinStack({0.0f, 0.25f, -0.4f}, parentObject);
}

void StoreMenuView::CreateCoinsFor50CoinsProduct(Pht::SceneObject& parentObject) {
    CreateCoin({-0.5f, 0.0f, 0.38f}, parentObject);
    CreateCoin({0.4f, 0.0f, 0.35f}, parentObject);
    CreateCoin({-0.25f, 0.13f, 0.38f}, parentObject);
    CreateTallCoinStack({-0.35f, 0.5f, -0.4f}, parentObject);
    CreateShortCoinStack({0.5f, 0.25f, -0.4f}, parentObject);
}

void StoreMenuView::CreateCoinsFor100CoinsProduct(Pht::SceneObject& parentObject) {
    auto& coinPile {CreateSceneObject()};
    coinPile.SetRenderable(mCoinPileRenderable.get());
    coinPile.GetTransform().SetScale(1.1f);
    parentObject.AddChild(coinPile);
}

void StoreMenuView::CreateCoinsFor250CoinsProduct(Pht::SceneObject& parentObject) {
    auto& coinPile {CreateSceneObject()};
    coinPile.SetRenderable(mCoinPileRenderable.get());
    auto& transform {coinPile.GetTransform()};
    transform.SetPosition({0.0f, 0.0f, 0.3f});
    transform.SetScale(1.1f);
    parentObject.AddChild(coinPile);
    
    CreateTallerCoinStack({-0.35f, 0.75f, -0.6f}, parentObject);
    CreateTallerCoinStack({0.5f, 0.55f, -0.6f}, parentObject);
}

void StoreMenuView::CreateCoinsFor500CoinsProduct(Pht::SceneObject& parentObject) {
    auto& coinPile {CreateSceneObject()};
    coinPile.SetRenderable(mCoinPileRenderable.get());
    auto& transform {coinPile.GetTransform()};
    transform.SetPosition({0.0f, 0.0f, 0.3f});
    transform.SetScale(1.1f);
    parentObject.AddChild(coinPile);
    
    CreateTallerCoinStack({-0.95f, 0.55f, -0.4f}, parentObject);
    CreateTallerCoinStack({-0.45f, 0.85f, -0.6f}, parentObject);
    CreateTallerCoinStack({0.0f, 0.65f, -0.6f}, parentObject);
    CreateTallerCoinStack({0.75f, 0.45f, -0.6f}, parentObject);
    CreateTallerCoinStack({0.45f, 0.85f, -0.9f}, parentObject);
}

void StoreMenuView::CreateCoin(const Pht::Vec3& position, Pht::SceneObject& parentObject) {
    auto& coin {CreateSceneObject()};
    coin.SetRenderable(mLightCoinRenderable.get());
    auto& transform {coin.GetTransform()};
    transform.SetPosition(position);
    transform.SetRotation({-90.0f, 0.0f, 0.0f});
    transform.SetScale(0.7f);
    parentObject.AddChild(coin);
}

void StoreMenuView::CreateShortCoinStack(const Pht::Vec3& position, Pht::SceneObject& parentObject) {
    auto& coinStack {CreateSceneObject()};
    coinStack.SetRenderable(mShortCoinStackRenderable.get());
    coinStack.GetTransform().SetPosition(position);
    parentObject.AddChild(coinStack);
    CreateCoin(position + Pht::Vec3{0.05f, 0.315f, 0.02f}, parentObject);
}

void StoreMenuView::CreateTallCoinStack(const Pht::Vec3& position, Pht::SceneObject& parentObject) {
    auto& coinStack {CreateSceneObject()};
    coinStack.SetRenderable(mTallCoinStackRenderable.get());
    coinStack.GetTransform().SetPosition(position);
    parentObject.AddChild(coinStack);
    CreateCoin(position + Pht::Vec3{0.05f, 0.565f, 0.02f}, parentObject);
}

void StoreMenuView::CreateTallerCoinStack(const Pht::Vec3& position, Pht::SceneObject& parentObject) {
    auto& coinStack {CreateSceneObject()};
    coinStack.SetRenderable(mTallerCoinStackRenderable.get());
    coinStack.GetTransform().SetPosition(position);
    parentObject.AddChild(coinStack);
}

void StoreMenuView::CreateGlowEffect(Pht::IEngine& engine,
                                     Pht::SceneObject& parentObject,
                                     Product& product,
                                     const Pht::Vec2& size) {
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
        .mColor = Pht::Vec4{0.43f, 0.43f, 0.43f, 1.0f},
        .mColorRandomPart = Pht::Vec4{0.0f, 0.0f, 0.0f, 0.0f},
        .mTextureFilename = "flare24.png",
        .mTimeToLive = std::numeric_limits<float>::infinity(),
        .mTimeToLiveRandomPart = 0.0f,
        .mFadeOutDuration = 0.5f,
        .mZAngularVelocity = 20.0f,
        .mZAngularVelocityRandomPart = 10.0f,
        .mSize = size,
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.0f,
        .mGrowDuration = 0.1f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    product.mGlowEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                         particleEmitterSettings,
                                                                         Pht::RenderMode::Triangles);
    auto& material {product.mGlowEffect->GetRenderable()->GetMaterial()};
    material.SetShaderType(Pht::ShaderType::ParticleNoAlphaTexture);
    
    product.mGlowEffect->GetTransform().SetPosition({0.0f, 0.9f, -1.9f});
    parentObject.AddChild(*product.mGlowEffect);
}

void StoreMenuView::CreateTwinklesEffect(Pht::IEngine& engine,
                                         Pht::SceneObject& parentObject,
                                         Product& product,
                                         const Pht::Vec3& twinklesVolume) {
    Pht::EmitterSettings particleEmitterSettings {
        .mPosition = Pht::Vec3{0.0f, 0.0f, 0.0f},
        .mSize = twinklesVolume,
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
        .mSize = Pht::Vec2{2.7f, 2.7f},
        .mSizeRandomPart = 0.0f,
        .mShrinkDuration = 0.4f
    };
    
    auto& particleSystem {engine.GetParticleSystem()};
    product.mTwinklesEffect = particleSystem.CreateParticleEffectSceneObject(particleSettings,
                                                                             particleEmitterSettings,
                                                                             Pht::RenderMode::Triangles);
    product.mTwinklesEffect->GetTransform().SetPosition({0.0f, 1.0f, UiLayer::root});
    parentObject.AddChild(*product.mTwinklesEffect);
}

void StoreMenuView::Init() {
    mAnimationTime = 0.0f;
    
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
        
        product.mCoinPilesAndStacks->GetTransform().SetRotation({coinPilesXAngle, yAngle, 0.0f});

        product.mGlowEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
        product.mTwinklesEffect->GetComponent<Pht::ParticleEffect>()->Update(dt);
    }
}
