#ifndef CommonResources_hpp
#define CommonResources_hpp

#include <memory>

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
    };
}

#endif
