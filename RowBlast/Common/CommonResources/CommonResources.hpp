#ifndef CommonResources_hpp
#define CommonResources_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"

// Game includes.
#include "Materials.hpp"
#include "Font.hpp"

namespace Pht {
    class IEngine;
    class IAudio;
}

namespace RowBlast {
    enum class PotentiallyZoomedScreen {
        Yes,
        No
    };
    
    class CommonResources {
    public:
        explicit CommonResources(Pht::IEngine& engine);
        
        const Pht::Font& GetHussarFontSize22(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::Font& GetHussarFontSize27(PotentiallyZoomedScreen potentiallyZoomed) const;
        const Pht::Font& GetHussarFontSize35(PotentiallyZoomedScreen potentiallyZoomed) const;
        
        const Pht::Font& GetHussarFontSize52PotentiallyZoomedScreen() const {
            return *mHussarFontSize52PotentiallyZoomedScreen;
        }
        
        const Pht::Vec2& GetOrthographicFrustumSizePotentiallyZoomedScreen() const {
            return mOrthographicFrustumSizePotentiallyZoomedScreen;
        }

        const Pht::Vec2& GetHudFrustumSizePotentiallyZoomedScreen() const {
            return mHudFrustumSizePotentiallyZoomedScreen;
        }
        
        float GetTopPaddingPotentiallyZoomedScreen() const {
            return mTopPaddingPotentiallyZoomedScreen;
        }

        float GetBottomPaddingPotentiallyZoomedScreen() const {
            return mBottomPaddingPotentiallyZoomedScreen;
        }

        const Materials& GetMaterials() const {
            return mMaterials;
        }

        static const std::string mBlipSound;
        static const std::string mBombSound;
        static constexpr auto narrowFrustumHeightFactor {1.11f};
        
    private:
        void AddSounds(Pht::IAudio& audio);
        
        Materials mMaterials;
        std::unique_ptr<Pht::Font> mHussarFontSize22;
        std::unique_ptr<Pht::Font> mHussarFontSize27;
        std::unique_ptr<Pht::Font> mHussarFontSize35;
        std::unique_ptr<Pht::Font> mHussarFontSize22PotentiallyZoomedScreen;
        std::unique_ptr<Pht::Font> mHussarFontSize27PotentiallyZoomedScreen;
        std::unique_ptr<Pht::Font> mHussarFontSize35PotentiallyZoomedScreen;
        std::unique_ptr<Pht::Font> mHussarFontSize52PotentiallyZoomedScreen;
        Pht::Vec2 mOrthographicFrustumSizePotentiallyZoomedScreen;
        Pht::Vec2 mHudFrustumSizePotentiallyZoomedScreen;
        float mTopPaddingPotentiallyZoomedScreen {0.0f};
        float mBottomPaddingPotentiallyZoomedScreen {0.0f};
    };
}

#endif
