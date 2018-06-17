#include "MapHud.hpp"

// Engine includes.
#include "IEngine.hpp"
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "Font.hpp"
#include "TextComponent.hpp"

// Game includes.
#include "UserData.hpp"
#include "StringUtils.hpp"
#include "GradientRectangle.hpp"
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto countdownNumChars {5};
}

MapHud::MapHud(Pht::IEngine& engine,
               const UserData& userData,
               const Pht::Font& font,
               Pht::Scene& scene,
               int hudLayer) :
    mUserData {userData} {
    
    auto& hudObject {scene.CreateSceneObject()};
    hudObject.SetLayer(hudLayer);
    scene.GetRoot().AddChild(hudObject);
    
    Pht::TextProperties textProperties {
        font,
        1.0f,
        Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f},
        Pht::TextShadow::Yes,
        {0.05f, 0.05f},
        {0.4f, 0.4f, 0.4f, 0.5f}
    };
    
    CreateLivesObject(engine, scene, hudObject, textProperties);
    CreateNewLifeCountdownObject(engine, scene, hudObject, textProperties);
}

void MapHud::CreateLivesObject(Pht::IEngine& engine,
                               Pht::Scene& scene,
                               Pht::SceneObject& parentObject,
                               const Pht::TextProperties& textProperties) {
    auto& renderer {engine.GetRenderer()};
    auto& hudFrustumSize {renderer.GetHudFrustumSize()};
    
    Pht::Vec3 livesContainerPosition {
        -hudFrustumSize.x / 2.0f + 2.3f,
        hudFrustumSize.y / 2.0f - 0.8625f - renderer.GetTopPaddingHeight(),
        UiLayer::root
    };

    auto& livesContainer {scene.CreateSceneObject()};
    livesContainer.GetTransform().SetPosition(livesContainerPosition);
    parentObject.AddChild(livesContainer);

    Pht::Vec3 rectanglePosition {0.0f, 0.0f, UiLayer::lowerTextRectangle};
    CreateTextRectangle(engine, scene, livesContainer, rectanglePosition);

    // Warning! Must be three spaces to fit digits.
    mLivesText = &scene.CreateText("LIVES   ", textProperties);

    mLivesText->GetSceneObject().GetTransform().SetPosition({-1.1f, -0.23f, UiLayer::text});
    livesContainer.AddChild(mLivesText->GetSceneObject());
}

void MapHud::CreateNewLifeCountdownObject(Pht::IEngine& engine,
                                          Pht::Scene& scene,
                                          Pht::SceneObject& parentObject,
                                          const Pht::TextProperties& textProperties) {
    auto& renderer {engine.GetRenderer()};
    auto& hudFrustumSize {renderer.GetHudFrustumSize()};

    Pht::Vec3 newLifeCountdownContainerPosition {
        -hudFrustumSize.x / 2.0f + 2.3f,
        hudFrustumSize.y / 2.0f - 2.11f - renderer.GetTopPaddingHeight(),
        UiLayer::root
    };

    mNewLifeCountdownContainer = &scene.CreateSceneObject();
    mNewLifeCountdownContainer->GetTransform().SetPosition(newLifeCountdownContainerPosition);
    parentObject.AddChild(*mNewLifeCountdownContainer);

    Pht::Vec3 rectanglePosition {0.0f, 0.0f, UiLayer::lowerTextRectangle};
    CreateTextRectangle(engine, scene, *mNewLifeCountdownContainer, rectanglePosition);
    
    mNewLifeCountdownText = &scene.CreateText("00:00", textProperties);
    mNewLifeCountdownText->GetSceneObject().GetTransform().SetPosition({-0.9f, -0.23f, UiLayer::text});
    mNewLifeCountdownContainer->AddChild(mNewLifeCountdownText->GetSceneObject());
}

void MapHud::CreateTextRectangle(Pht::IEngine& engine,
                                 Pht::Scene& scene,
                                 Pht::SceneObject& parentObject,
                                 const Pht::Vec3& position) {
    Pht::Vec2 size {4.0f, 0.7f};
    auto leftQuadWidth {1.0f};
    auto rightQuadWidth {1.0f};
    
    GradientRectangleColors colors {
        .mLeft = {0.6f, 0.3f, 0.75f, 0.0f},
        .mMid = {0.6f, 0.3f, 0.75f, 0.8f},
        .mRight = {0.6f, 0.3f, 0.75f, 0.0f}
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

void MapHud::Update() {
    UpdateLivesText();
    UpdateCountdown();
    
    if (mUserData.GetLifeManager().HasFullNumLives()) {
        mNewLifeCountdownContainer->SetIsVisible(false);
    } else {
        mNewLifeCountdownContainer->SetIsVisible(true);
    }
}

void MapHud::UpdateLivesText() {
    auto lives {mUserData.GetLifeManager().GetNumLives()};
    
    if (lives != mNumLives) {
        const auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%1d", lives);
        auto numDigits {std::strlen(buffer)};
        assert(numDigits <= 1);
        
        auto& text {mLivesText->GetText()};
        auto textLength {text.size()};
        assert(textLength == 8);
        text[textLength - 1] = buffer[0];
        
        mNumLives = lives;
    }
}

void MapHud::UpdateCountdown() {
    auto secondsUntilNewLife {mUserData.GetLifeManager().GetDurationUntilNewLife()};

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
