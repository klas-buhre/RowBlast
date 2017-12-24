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

        const Pht::Material& GetLightGrayMaterial() const {
            return *mLightGrayMaterial;
        }

        const Pht::Material& GetYellowMaterial() const {
            return *mYellowMaterial;
        }
        
        const Pht::Material& GetSkyMaterial() const {
            return *mSkyMaterial;
        }

        const Pht::Material& GetGoldFieldBlockMaterial() const {
            return *mGoldFieldBlockMaterial;
        }

        const Pht::Material& GetRedFieldBlockMaterial() const {
            return *mRedFieldBlockMaterial;
        }

        const Pht::Material& GetBlueFieldBlockMaterial() const {
            return *mBlueFieldBlockMaterial;
        }

        const Pht::Material& GetGreenFieldBlockMaterial() const {
            return *mGreenFieldBlockMaterial;
        }

        const Pht::Material& GetGrayFieldBlockMaterial() const {
            return *mGrayFieldBlockMaterial;
        }

        const Pht::Material& GetDarkGrayFieldBlockMaterial() const {
            return *mDarkGrayFieldBlockMaterial;
        }

        const Pht::Material& GetYellowFieldBlockMaterial() const {
            return *mYellowFieldBlockMaterial;
        }

    private:
        void CreateGoldMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateBlueMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateRedMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateGreenMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateLightGrayMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateYellowMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateSkyMaterial();
        void CreateGoldFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateRedFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateBlueFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateGreenFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateGrayFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateDarkGrayFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateYellowFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        
        std::unique_ptr<Pht::Material> mGoldMaterial;
        std::unique_ptr<Pht::Material> mRedMaterial;
        std::unique_ptr<Pht::Material> mBlueMaterial;
        std::unique_ptr<Pht::Material> mGreenMaterial;
        std::unique_ptr<Pht::Material> mLightGrayMaterial;
        std::unique_ptr<Pht::Material> mYellowMaterial;
        std::unique_ptr<Pht::Material> mSkyMaterial;
        std::unique_ptr<Pht::Material> mGoldFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mRedFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mBlueFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mGreenFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mGrayFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mDarkGrayFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mYellowFieldBlockMaterial;
    };
}

#endif
