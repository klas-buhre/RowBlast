#ifndef MapHud_hpp
#define MapHud_hpp

#include <chrono>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
    class Font;
    class TextComponent;
    class TextProperties;
}

namespace RowBlast {
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
        void CreateLivesObject(Pht::IEngine& engine,
                               Pht::Scene& scene,
                               Pht::SceneObject& parentObject,
                               const Pht::TextProperties& textProperties);
        void CreateNewLifeCountdownObject(Pht::IEngine& engine,
                                          Pht::Scene& scene,
                                          Pht::SceneObject& parentObject,
                                          const Pht::TextProperties& textProperties);
        void CreateCountdownRectangle(Pht::IEngine& engine,
                                      Pht::Scene& scene,
                                      Pht::SceneObject& parentObject,
                                      const Pht::Vec3& position);
        void UpdateLivesText();
        void UpdateCountdown();
        
        const UserData& mUserData;
        Pht::TextComponent* mLivesText {nullptr};
        Pht::TextComponent* mNewLifeCountdownText {nullptr};
        Pht::SceneObject* mNewLifeCountdownContainer {nullptr};
        int mNumLives {-1};
        std::chrono::seconds mSecondsUntilNewLife;
    };
}

#endif
