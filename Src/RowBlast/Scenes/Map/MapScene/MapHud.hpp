#ifndef MapHud_hpp
#define MapHud_hpp

#include <chrono>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class IEngine;
    class Scene;
    class SceneObject;
    class TextComponent;
    class TextProperties;
}

namespace RowBlast {
    class UserServices;
    class CommonResources;
    
    class MapHud {
    public:
        MapHud(Pht::IEngine& engine,
               const UserServices& userServices,
               const CommonResources& commonResources,
               Pht::Scene& scene,
               int hudLayer);
        
        void Update();
        
    private:
        void CreateLightAndCamera(Pht::Scene& scene,
                                  Pht::SceneObject& parentObject,
                                  int hudLayer);
        void CreateLivesObject(Pht::Scene& scene,
                               Pht::SceneObject& parentObject,
                               const CommonResources& commonResources);
        void CreateNewLifeCountdownObject(Pht::Scene& scene,
                                          Pht::SceneObject& parentObject,
                                          const CommonResources& commonResources);
        void CreateCoinsObject(Pht::Scene& scene,
                               Pht::SceneObject& hudObject,
                               const CommonResources& commonResources);
        void UpdateLivesText();
        void UpdateCoinsText();
        void UpdateCountdown();
        
        Pht::IEngine& mEngine;
        const UserServices& mUserServices;
        Pht::TextComponent* mLivesText {nullptr};
        Pht::TextComponent* mNewLifeCountdownText {nullptr};
        Pht::TextComponent* mCoinsText {nullptr};
        Pht::SceneObject* mNewLifeCountdownContainer {nullptr};
        int mNumLives {-1};
        int mNumCoins {-1};
        std::chrono::seconds mSecondsUntilNewLife;
        Pht::Vec3 mCoinRotation;
    };
}

#endif
