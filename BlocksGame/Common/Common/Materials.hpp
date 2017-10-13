#ifndef Materials_hpp
#define Materials_hpp

#include <memory>

// Engine includes.
#include "Material.hpp"

namespace Pht {
    class IEngine;
}

namespace BlocksGame {
    class Materials {
    public:
        explicit Materials(Pht::IEngine& engine);
        
        const Pht::Material& GetGoldMaterial() const {
            return *mGoldMaterial;
        }

        const Pht::Material& GetRedMaterial() const {
            return *mRedMaterial;
        }

        const Pht::Material& GetBlueMaterial() const {
            return *mBlueMaterial;
        }
        
        const Pht::Material& GetGreenMaterial() const {
            return *mGreenMaterial;
        }

        const Pht::Material& GetGrayMaterial() const {
            return *mGrayMaterial;
        }
        
        const Pht::Material& GetDarkGrayMaterial() const {
            return *mDarkGrayMaterial;
        }

        const Pht::Material& GetLightGrayMaterial() const {
            return *mLightGrayMaterial;
        }

        const Pht::Material& GetYellowMaterial() const {
            return *mYellowMaterial;
        }
        
        const Pht::Material& GetSkyMaterial() const {
            return *mSkyMaterial;
        }
        
    private:
        void CreateGoldMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateBlueMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateRedMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateGreenMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateGrayMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateDarkGrayMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateLightGrayMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateYellowMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateSkyMaterial();
        
        std::unique_ptr<Pht::Material> mGoldMaterial;
        std::unique_ptr<Pht::Material> mRedMaterial;
        std::unique_ptr<Pht::Material> mBlueMaterial;
        std::unique_ptr<Pht::Material> mGreenMaterial;
        std::unique_ptr<Pht::Material> mGrayMaterial;
        std::unique_ptr<Pht::Material> mDarkGrayMaterial;
        std::unique_ptr<Pht::Material> mLightGrayMaterial;
        std::unique_ptr<Pht::Material> mYellowMaterial;
        std::unique_ptr<Pht::Material> mSkyMaterial;
    };
}

#endif
