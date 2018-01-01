#ifndef MapHud_hpp
#define MapHud_hpp

#include <chrono>

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
    class Font;
    class TextComponent;
}

namespace BlocksGame {
    class UserData;
    
    class MapHud {
    public:
        MapHud(Pht::IEngine& engine,
               const UserData& userData,
               const Pht::Font& font,
               Pht::Scene& scene,
               int hudLayer);
        
        void Update();
        
    private:
        void CreateLivesRectangle(Pht::IEngine& engine,
                                  Pht::Scene& scene,
                                  Pht::SceneObject& parentObject);
        void CreateCountdownRectangle(Pht::IEngine& engine,
                                      Pht::Scene& scene,
                                      Pht::SceneObject& parentObject);
        void UpdateLivesText();
        void UpdateCountdown();
        
        const UserData& mUserData;
        Pht::TextComponent* mLivesText {nullptr};
        Pht::TextComponent* mNewLifeCountdownText {nullptr};
        Pht::SceneObject* mNewLifeCountdownRectangle {nullptr};
        int mNumLives {-1};
        std::chrono::seconds mSecondsUntilNewLife;
    };
}

#endif
