#ifndef Materials_hpp
#define Materials_hpp

#include <memory>

// Engine includes.
#include "Material.hpp"

namespace Pht {
    class IEngine;
}

namespace RowBlast {
    class Materials {
    public:
        explicit Materials(Pht::IEngine& engine);
        
        const Pht::EnvMapTextureFilenames& GetEnvMapTextureFilenames() const {
            return mEnvMapTextureFilenames;
        }
        
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
        
        const Pht::Material& GetGrayYellowMaterial() const {
            return *mGrayYellowMaterial;
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

        const Pht::Material& GetYellowFieldBlockMaterial() const {
            return *mYellowFieldBlockMaterial;
        }

    private:
        void CreateGoldMaterial();
        void CreateBlueMaterial();
        void CreateRedMaterial();
        void CreateGreenMaterial();
        void CreateLightGrayMaterial();
        void CreateGrayYellowMaterial();
        void CreateRedFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateBlueFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateGreenFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateGrayFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        void CreateYellowFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures);
        
        Pht::EnvMapTextureFilenames mEnvMapTextureFilenames;
        std::unique_ptr<Pht::Material> mGoldMaterial;
        std::unique_ptr<Pht::Material> mRedMaterial;
        std::unique_ptr<Pht::Material> mBlueMaterial;
        std::unique_ptr<Pht::Material> mGreenMaterial;
        std::unique_ptr<Pht::Material> mLightGrayMaterial;
        std::unique_ptr<Pht::Material> mGrayYellowMaterial;
        std::unique_ptr<Pht::Material> mRedFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mBlueFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mGreenFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mGrayFieldBlockMaterial;
        std::unique_ptr<Pht::Material> mYellowFieldBlockMaterial;
    };
}

#endif
