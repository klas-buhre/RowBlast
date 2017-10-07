#include "MapHud.hpp"

#include <assert.h>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"

// Game includes.
#include "UserData.hpp"
#include "StringUtils.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto livesRectangleDistFromBorder {2.4f};
    constexpr auto livesTextDistFromBorder {1.3f};
    constexpr auto countdownTextDistFromBorder {1.45f};
    constexpr auto countdownNumChars {5};
}

MapHud::MapHud(Pht::IEngine& engine, const UserData& userData) :
    mUserData {userData},
    mFont {"HussarBoldWeb.otf", engine.GetRenderer().GetAdjustedNumPixels(22)},
    mTextProperties {mFont, 1.0f, Pht::Vec4{1.0f, 1.0f, 1.0f, 1.0f}},
    mLivesText {
        {-engine.GetRenderer().GetHudFrustumSize().x / 2.0f + livesTextDistFromBorder, 12.65f},
        "LIVES",
        mTextProperties
    },
    mNewLifeCountdownText {
        {-engine.GetRenderer().GetHudFrustumSize().x / 2.0f + countdownTextDistFromBorder, 12.0f},
        "00:00",
        mTextProperties} {

    mLivesText.mText += "   "; // Warning! Must be three spaces to fit digits.
    
    CreateLivesRectangle(engine);
    CreateCountdownRectangle(engine);
}

void MapHud::CreateLivesRectangle(Pht::IEngine& engine) {
    Pht::Vec3 position {
        -engine.GetRenderer().GetHudFrustumSize().x / 2.0f + livesRectangleDistFromBorder,
        12.87f,
        0.0f
    };
    
    Pht::Vec2 size {4.0f, 0.7f};
    float leftQuadWidth {1.0f};
    float rightQuadWidth {1.0f};
    
    GradientRectangle::Colors colors {
        .mLeft = {0.6f, 0.3f, 0.75f, 0.0f},
        .mMid = {0.6f, 0.3f, 0.75f, 0.8f},
        .mRight = {0.6f, 0.3f, 0.75f, 0.0f}
    };
    
    mLivesRectangle = std::make_unique<GradientRectangle>(engine,
                                                          position,
                                                          size,
                                                          0.0f,
                                                          leftQuadWidth,
                                                          rightQuadWidth,
                                                          colors,
                                                          colors);
}

void MapHud::CreateCountdownRectangle(Pht::IEngine& engine) {
    Pht::Vec3 position {
        -engine.GetRenderer().GetHudFrustumSize().x / 2.0f + livesRectangleDistFromBorder,
        12.22f,
        0.0f
    };
    
    Pht::Vec2 size {4.0f, 0.7f};
    float leftQuadWidth {1.0f};
    float rightQuadWidth {1.0f};
    
    GradientRectangle::Colors colors {
        .mLeft = {0.6f, 0.3f, 0.75f, 0.0f},
        .mMid = {0.6f, 0.3f, 0.75f, 0.8f},
        .mRight = {0.6f, 0.3f, 0.75f, 0.0f}
    };
    
    mNewLifeCountdownRectangle = std::make_unique<GradientRectangle>(engine,
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
}

void MapHud::UpdateLivesText() {
    auto lives {mUserData.GetLifeManager().GetNumLives()};
    
    if (lives != mNumLives) {
        const auto bufSize {64};
        char buffer[bufSize];
        std::snprintf(buffer, bufSize, "%1d", lives);
        auto numDigits {std::strlen(buffer)};
        assert(numDigits <= 1);
        
        auto& text {mLivesText.mText};
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
        
        auto& text {mNewLifeCountdownText.mText};
        auto textLength {text.size()};
        assert(textLength == countdownNumChars);
        
        StringUtils::StringCopy(text, 0, countdownBuffer, countdownNumChars);
        
        mSecondsUntilNewLife = secondsUntilNewLife;
    }
}
