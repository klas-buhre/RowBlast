#ifndef CommonResources_hpp
#define CommonResources_hpp

// Game includes.
#include "Materials.hpp"
#include "Font.hpp"

namespace Pht {
    class IEngine;
    class IAudio;
}

namespace BlocksGame {
    class CommonResources {
    public:
        explicit CommonResources(Pht::IEngine& engine);
        
        const Materials& GetMaterials() const {
            return mMaterials;
        }

        const Pht::Font& GetHussarFontSize27() const {
            return mHussarFontSize27;
        }

        const Pht::Font& GetHussarFontSize30() const {
            return mHussarFontSize30;
        }
        
        static const std::string mBlipSound;
        static const std::string mBombSound;
        
    private:
        void AddSounds(Pht::IAudio& audio);
        
        Materials mMaterials;
        Pht::Font mHussarFontSize27;
        Pht::Font mHussarFontSize30;
    };
}

#endif
