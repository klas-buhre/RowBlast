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

using namespace BlocksGame;

namespace {
    constexpr auto livesRectangleDistFromBorder {2.4f};
    constexpr auto livesTextDistFromBorder {1.3f};
    constexpr auto countdownTextDistFromBorder {1.45f};
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
    
    Pht::TextProperties textProperties {font, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}};
    
    // Warning! Must be three spaces to fit digits.
    mLivesText = &scene.CreateText("LIVES   ", textProperties);
    
    Pht::Vec3 livesTextPosition {
        -engine.GetRenderer().GetHudFrustumSize().x / 2.0f + livesTextDistFromBorder,
        12.65f,
        UiLayer::text
    };
    
    mLivesText->GetSceneObject().GetTransform().SetPosition(livesTextPosition);
    hudObject.AddChild(mLivesText->GetSceneObject());
    
    Pht::Vec3 newLifeCountdownTextPosition {
        -engine.GetRenderer().GetHudFrustumSize().x / 2.0f + countdownTextDistFromBorder,
        12.0f,
        UiLayer::text
    };
    
    mNewLifeCountdownText = &scene.CreateText("00:00", textProperties);
    mNewLifeCountdownText->GetSceneObject().GetTransform().SetPosition(newLifeCountdownTextPosition);
    hudObject.AddChild(mNewLifeCountdownText->GetSceneObject());
    
    CreateLivesRectangle(engine, scene, hudObject);
    CreateCountdownRectangle(engine, scene, hudObject);
}

void MapHud::CreateLivesRectangle(Pht::IEngine& engine,
                                  Pht::Scene& scene,
                                  Pht::SceneObject& parentObject) {
    Pht::Vec3 position {
        -engine.GetRenderer().GetHudFrustumSize().x / 2.0f + livesRectangleDistFromBorder,
        12.87f,
        UiLayer::textRectangle
    };
    
    Pht::Vec2 size {4.0f, 0.7f};
    float leftQuadWidth {1.0f};
    float rightQuadWidth {1.0f};
    
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

void MapHud::CreateCountdownRectangle(Pht::IEngine& engine,
                                      Pht::Scene& scene,
                                      Pht::SceneObject& parentObject) {
    Pht::Vec3 position {
        -engine.GetRenderer().GetHudFrustumSize().x / 2.0f + livesRectangleDistFromBorder,
        12.22f,
        UiLayer::textRectangle
    };
    
    Pht::Vec2 size {4.0f, 0.7f};
    float leftQuadWidth {1.0f};
    float rightQuadWidth {1.0f};
    
    GradientRectangleColors colors {
        .mLeft = {0.6f, 0.3f, 0.75f, 0.0f},
        .mMid = {0.6f, 0.3f, 0.75f, 0.8f},
        .mRight = {0.6f, 0.3f, 0.75f, 0.0f}
    };
    
    mNewLifeCountdownRectangle = &CreateGradientRectangle(scene,
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
        mNewLifeCountdownRectangle->SetIsVisible(false);
        mNewLifeCountdownText->GetSceneObject().SetIsVisible(false);
    } else {
        mNewLifeCountdownRectangle->SetIsVisible(true);
        mNewLifeCountdownText->GetSceneObject().SetIsVisible(true);
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
