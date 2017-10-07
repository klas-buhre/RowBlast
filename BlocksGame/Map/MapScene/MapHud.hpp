#ifndef MapHud_hpp
#define MapHud_hpp

#include <memory>
#include <chrono>

// Engine includes.
#include "Vector.hpp"
#include "Font.hpp"

// Game includes.
#include "GradientRectangle.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class UserData;
    
    class MapHud {
    public:
        MapHud(Pht::IEngine& engine, const UserData& userData);
        
        void Update();
        
        const Pht::Text& GetLivesText() const {
            return mLivesText;
        }

        const GradientRectangle& GetLivesRectangle() const {
            return *mLivesRectangle;
        }

        const Pht::Text& GetNewLifeCountdownText() const {
            return mNewLifeCountdownText;
        }

        const GradientRectangle& GettNewLifeCountdownRectangle() const {
            return *mNewLifeCountdownRectangle;
        }
        
    private:
        void CreateLivesRectangle(Pht::IEngine& engine);
        void CreateCountdownRectangle(Pht::IEngine& engine);
        void UpdateLivesText();
        void UpdateCountdown();
        
        const UserData& mUserData;
        Pht::Font mFont;
        Pht::TextProperties mTextProperties;
        Pht::Text mLivesText;
        std::unique_ptr<GradientRectangle> mLivesRectangle;
        Pht::Text mNewLifeCountdownText;
        std::unique_ptr<GradientRectangle> mNewLifeCountdownRectangle;
        int mNumLives {-1};
        std::chrono::seconds mSecondsUntilNewLife;
    };
}

#endif
