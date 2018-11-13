#include "MapHud.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "ObjMesh.hpp"
#include "TextComponent.hpp"
#include "LightComponent.hpp"
#include "CameraComponent.hpp"

// Game includes.
#include "CommonResources.hpp"
#include "UserServices.hpp"
#include "StringUtils.hpp"
#include "GradientRectangle.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto countdownNumChars {5};
    constexpr auto coinRotationSpeed {60.0f};
}

MapHud::MapHud(Pht::IEngine& engine,
               const UserServices& userServices,
               const CommonResources& commonResources,
               Pht::Scene& scene,
               int hudLayer) :
    mEngine {engine},
    mUserServices {userServices},
    mCoinRotation {0.0f, 0.0f, 0.0f} {
    
    auto& hudObject {scene.CreateSceneObject()};
    hudObject.SetLayer(hudLayer);
    scene.GetRoot().AddChild(hudObject);
    
    CreateLightAndCamera(scene, hudObject, hudLayer);
    CreateLivesObject(scene, hudObject, commonResources);
    CreateNewLifeCountdownObject(scene, hudObject, commonResources);
    CreateCoinsObject(scene, hudObject, commonResources);
}

void MapHud::CreateLightAndCamera(Pht::Scene& scene,
                                  Pht::SceneObject& parentObject,
                                  int hudLayer) {
    auto& lightSceneObject {scene.CreateSceneObject()};
    lightSceneObject.SetIsVisible(false);
    auto lightComponent {std::make_unique<Pht::LightComponent>(lightSceneObject)};
    lightComponent->SetDirectionalIntensity(1.35f);
    auto* light {lightComponent.get()};
    lightSceneObject.SetComponent<Pht::LightComponent>(std::move(lightComponent));
    parentObject.AddChild(lightSceneObject);
    
    auto& cameraSceneObject {scene.CreateSceneObject()};
    cameraSceneObject.SetIsVisible(false);
    cameraSceneObject.GetTransform().SetPosition({0.0f, 0.0f, 15.5f});
    auto cameraComponent {std::make_unique<Pht::CameraComponent>(cameraSceneObject)};
    auto* camera {cameraComponent.get()};
    cameraSceneObject.SetComponent<Pht::CameraComponent>(std::move(cameraComponent));
    parentObject.AddChild(cameraSceneObject);
    
    auto* hudRenderPass {scene.GetRenderPass(hudLayer)};
    assert(hudRenderPass);
    hudRenderPass->SetLight(light);
    hudRenderPass->SetCamera(camera);
}

void MapHud::CreateLivesObject(Pht::Scene& scene,
                               Pht::SceneObject& parentObject,
                               const CommonResources& commonResources) {
    auto& livesContainer {scene.CreateSceneObject()};
    auto& renderer {mEngine.GetRenderer()};
    auto& hudFrustumSize {renderer.GetHudFrustumSize()};
    
    Pht::Vec3 position {
        -hudFrustumSize.x / 2.0f + 2.7f,
        hudFrustumSize.y / 2.0f - 1.15f - renderer.GetTopPaddingHeight(),
        UiLayer::root
    };
    
    livesContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(livesContainer);

    Pht::Vec2 gradientRectangleSize {3.2f, 1.0f};
    auto leftQuadWidth {0.25f};
    auto rightQuadWidth {0.25f};

    GradientRectangleColors colors {
        .mLeft = {0.175f, 0.175f, 0.175f, 0.0f},
        .mMid = {0.175f, 0.175f, 0.175f, 0.75f},
        .mRight = {0.175f, 0.175f, 0.175f, 0.0f}
    };
    
    Pht::Vec3 gradientRectanglePosition {0.0f, 0.0f, UiLayer::lowerTextRectangle};
    CreateGradientRectangle(scene,
                            livesContainer,
                            gradientRectanglePosition,
                            gradientRectangleSize,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);

    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize20(PotentiallyZoomedScreen::No),
        1.1f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };
    
    mLivesText = &scene.CreateText("5", textProperties);
    auto& livesTextSceneObject {mLivesText->GetSceneObject()};
    livesTextSceneObject.GetTransform().SetPosition({-0.2f, -0.215f, UiLayer::text});
    livesContainer.AddChild(livesTextSceneObject);
    
    auto& heart {
        scene.CreateSceneObject(Pht::ObjMesh {"heart_112.obj", 3.25f},
                                commonResources.GetMaterials().GetRedMaterial())
    };
        
    heart.GetTransform().SetPosition({-1.5f, 0.0f, UiLayer::root});
    livesContainer.AddChild(heart);
}

void MapHud::CreateNewLifeCountdownObject(Pht::Scene& scene,
                                          Pht::SceneObject& parentObject,
                                          const CommonResources& commonResources) {
    auto& renderer {mEngine.GetRenderer()};
    auto& hudFrustumSize {renderer.GetHudFrustumSize()};

    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize20(PotentiallyZoomedScreen::No),
        1.1f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };

    Pht::Vec3 newLifeCountdownContainerPosition {
        -hudFrustumSize.x / 2.0f + 2.7f,
        hudFrustumSize.y / 2.0f - 2.25f - renderer.GetTopPaddingHeight(),
        UiLayer::root
    };

    mNewLifeCountdownContainer = &scene.CreateSceneObject();
    mNewLifeCountdownContainer->GetTransform().SetPosition(newLifeCountdownContainerPosition);
    parentObject.AddChild(*mNewLifeCountdownContainer);

    Pht::Vec3 rectanglePosition {0.0f, 0.0f, UiLayer::lowerTextRectangle};
    CreateTextRectangle(scene, *mNewLifeCountdownContainer, rectanglePosition);
    
    mNewLifeCountdownText = &scene.CreateText("00:00", textProperties);
    mNewLifeCountdownText->GetSceneObject().GetTransform().SetPosition({-0.9f, -0.23f, UiLayer::text});
    mNewLifeCountdownContainer->AddChild(mNewLifeCountdownText->GetSceneObject());
}

void MapHud::CreateTextRectangle(Pht::Scene& scene,
                                 Pht::SceneObject& parentObject,
                                 const Pht::Vec3& position) {
    Pht::Vec2 size {3.0f, 0.7f};
    auto leftQuadWidth {0.5f};
    auto rightQuadWidth {0.5f};

    GradientRectangleColors colors {
        .mLeft = {0.175f, 0.175f, 0.175f, 0.0f},
        .mMid = {0.175f, 0.175f, 0.175f, 0.75f},
        .mRight = {0.175f, 0.175f, 0.175f, 0.0f}
    };
    
    CreateGradientRectangle(scene,
                            parentObject,
                            position,
                            size,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);
}

void MapHud::CreateCoinsObject(Pht::Scene& scene,
                               Pht::SceneObject& parentObject,
                               const CommonResources& commonResources) {
    auto& coinsContainer {scene.CreateSceneObject()};
    auto& renderer {mEngine.GetRenderer()};
    auto& hudFrustumSize {renderer.GetHudFrustumSize()};
    
    Pht::Vec3 position {
        hudFrustumSize.x / 2.0f - 3.5f,
        hudFrustumSize.y / 2.0f - 1.15f - renderer.GetTopPaddingHeight(),
        UiLayer::root
    };
    
    coinsContainer.GetTransform().SetPosition(position);
    parentObject.AddChild(coinsContainer);

    Pht::Vec2 gradientRectangleSize {3.2f, 1.0f};
    auto leftQuadWidth {0.25f};
    auto rightQuadWidth {0.25f};

    GradientRectangleColors colors {
        .mLeft = {0.175f, 0.175f, 0.175f, 0.0f},
        .mMid = {0.175f, 0.175f, 0.175f, 0.75f},
        .mRight = {0.175f, 0.175f, 0.175f, 0.0f}
    };
    
    Pht::Vec3 gradientRectanglePosition {0.0f, 0.0f, UiLayer::lowerTextRectangle};
    CreateGradientRectangle(scene,
                            coinsContainer,
                            gradientRectanglePosition,
                            gradientRectangleSize,
                            0.0f,
                            leftQuadWidth,
                            rightQuadWidth,
                            colors,
                            colors);

    Pht::TextProperties textProperties {
        commonResources.GetHussarFontSize20(PotentiallyZoomedScreen::No),
        1.1f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}
    };
    
    // Warning! Must be five spaces to fit digits.
    mCoinsText = &scene.CreateText("   45", textProperties);
    auto& coinsTextSceneObject {mCoinsText->GetSceneObject()};
    coinsTextSceneObject.GetTransform().SetPosition({-0.7f, -0.215f, UiLayer::text});
    coinsContainer.AddChild(coinsTextSceneObject);
    
    mCoinSceneObject = &scene.CreateSceneObject(Pht::ObjMesh {"coin_852.obj", 3.15f},
                                                commonResources.GetMaterials().GetGoldMaterial());
    mCoinSceneObject->GetTransform().SetPosition({-1.5f, 0.0f, UiLayer::root});
    coinsContainer.AddChild(*mCoinSceneObject);
}

void MapHud::Update() {
    UpdateLivesText();
    UpdateCountdown();
    AnimateCoinRotation();
    
    if (mUserServices.GetLifeService().HasFullNumLives()) {
        mNewLifeCountdownContainer->SetIsVisible(false);
    } else {
        mNewLifeCountdownContainer->SetIsVisible(true);
    }
}

void MapHud::UpdateLivesText() {
    auto lives {mUserServices.GetLifeService().GetNumLives()};
    
    if (lives != mNumLives) {
        const auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%1d", lives);
        auto numDigits {std::strlen(buffer)};
        assert(numDigits <= 1);
        
        auto& text {mLivesText->GetText()};
        auto textLength {text.size()};
        assert(textLength == 1);
        text[0] = buffer[0];
        
        mNumLives = lives;
    }
}

void MapHud::UpdateCountdown() {
    auto secondsUntilNewLife {mUserServices.GetLifeService().GetDurationUntilNewLife()};

    if (secondsUntilNewLife != mSecondsUntilNewLife) {
        StaticStringBuffer countdownBuffer;
        StringUtils::FormatToMinutesAndSeconds(countdownBuffer, secondsUntilNewLife);
        
        auto numChars {std::strlen(countdownBuffer.data())};
        assert(numChars == countdownNumChars);
        
        auto& text {mNewLifeCountdownText->GetText()};
        auto textLength {text.size()};
        assert(textLength == countdownNumChars);
        
        StringUtils::StringCopy(text, 0, countdownBuffer, countdownNumChars);
        
        mSecondsUntilNewLife = secondsUntilNewLife;
    }
}

void MapHud::AnimateCoinRotation() {
    mCoinRotation.y += coinRotationSpeed * mEngine.GetLastFrameSeconds();
    
    if (mCoinRotation.y > 360.0f) {
        mCoinRotation.y -= 360.0f;
    }
    
    mCoinSceneObject->GetTransform().SetRotation(mCoinRotation);
}
