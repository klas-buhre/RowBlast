#ifndef CommonResources_hpp
#define CommonResources_hpp

#include <memory>

// Engine includes.
#include "Vector.hpp"
#include "Font.hpp"

// Game includes.
#include "Materials.hpp"
#include "GuiResources.hpp"
#include "GameHudRectangles.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class CommonResources {
    public:
        explicit CommonResources(Pht::IEngine& engine);
        
        const Pht::Font& GetHussarFontSize20(PotentiallyZoomedScreen potentiallyZoomed) const;
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
            return *mMaterials;
        }
        
        const GuiResources& GetGuiResources() const {
            return *mGuiResources;
        }

        const GameHudRectangles& GetGameHudRectangles() const {
            return *mGameHudRectangles;
        }
        
        float GetCellSize() const {
            return mCellSize;
        }

        static constexpr auto narrowFrustumHeightFactor {1.13f};
        
    private:
        std::unique_ptr<Materials> mMaterials;
        std::unique_ptr<GuiResources> mGuiResources;
        std::unique_ptr<GameHudRectangles> mGameHudRectangles;
        std::unique_ptr<Pht::Font> mHussarFontSize20;
        std::unique_ptr<Pht::Font> mHussarFontSize27;
        std::unique_ptr<Pht::Font> mHussarFontSize35;
        std::unique_ptr<Pht::Font> mHussarFontSize20PotentiallyZoomedScreen;
        std::unique_ptr<Pht::Font> mHussarFontSize27PotentiallyZoomedScreen;
        std::unique_ptr<Pht::Font> mHussarFontSize35PotentiallyZoomedScreen;
        std::unique_ptr<Pht::Font> mHussarFontSize52PotentiallyZoomedScreen;
        Pht::Vec2 mOrthographicFrustumSizePotentiallyZoomedScreen;
        Pht::Vec2 mHudFrustumSizePotentiallyZoomedScreen;
        float mTopPaddingPotentiallyZoomedScreen {0.0f};
        float mBottomPaddingPotentiallyZoomedScreen {0.0f};
        float mCellSize {1.25f};
    };
}

#endif
