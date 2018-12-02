#include "Materials.hpp"

using namespace RowBlast;

namespace {
    const auto reflectivity {0.84f};
    const auto fieldBlockReflectivity {0.825f};
}

Materials::Materials(Pht::IEngine& engine) :
    mEnvMapTextureFilenames {
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg",
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg",
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg"
    } {

    CreateGoldMaterial();
    CreateLightGoldMaterial();
    CreateLighterGoldMaterial();
    CreateBlueMaterial();
    CreateRedMaterial();
    CreateGreenMaterial();
    CreateLightGrayMaterial();
    CreateGrayYellowMaterial();
    
    Pht::EnvMapTextureFilenames fieldBlockEnvMapTextures {
        "sky_upside_down.jpg",
        "sky_upside_down.jpg",
        "sky_upside_down.jpg",
        "sky_upside_down.jpg",
        "sky_upside_down_patched_mirrored.jpg",
        "sky_upside_down.jpg"
    };
    
    CreateRedFieldBlockMaterial(fieldBlockEnvMapTextures);
    CreateBlueFieldBlockMaterial(fieldBlockEnvMapTextures);
    CreateGreenFieldBlockMaterial(fieldBlockEnvMapTextures);
    CreateGrayFieldBlockMaterial(fieldBlockEnvMapTextures);
    CreateYellowFieldBlockMaterial(fieldBlockEnvMapTextures);
}

void Materials::CreateGoldMaterial() {
    Pht::Color ambient {0.9f, 0.56f, 0.08f};
    Pht::Color diffuse {0.5f, 0.41f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mGoldMaterial = std::make_unique<Pht::Material>(mEnvMapTextureFilenames,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    0.6f);
    mGoldMaterial->GetDepthState().mDepthTestAllowedOverride = true;
}

void Materials::CreateLightGoldMaterial() {
    Pht::Color ambient {0.95f, 0.595f, 0.085f};
    Pht::Color diffuse {0.525f, 0.435f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mLightGoldMaterial = std::make_unique<Pht::Material>(mEnvMapTextureFilenames,
                                                         ambient,
                                                         diffuse,
                                                         specular,
                                                         shininess,
                                                         0.6f);
    mLightGoldMaterial->GetDepthState().mDepthTestAllowedOverride = true;
}

void Materials::CreateLighterGoldMaterial() {
    Pht::Color ambient {1.0f, 0.63f, 0.09f};
    Pht::Color diffuse {0.55f, 0.46f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mLighterGoldMaterial = std::make_unique<Pht::Material>(mEnvMapTextureFilenames,
                                                           ambient,
                                                           diffuse,
                                                           specular,
                                                           shininess,
                                                           0.6f);
    mLighterGoldMaterial->GetDepthState().mDepthTestAllowedOverride = true;
}

void Materials::CreateBlueMaterial() {
    Pht::Color ambient {0.05f, 0.5225f, 0.96f};
    Pht::Color diffuse {0.0f, 0.475f, 0.96f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mBlueMaterial = std::make_unique<Pht::Material>(mEnvMapTextureFilenames,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    reflectivity);
}

void Materials::CreateRedMaterial() {
    Pht::Color ambient {0.9f, 0.42f, 0.42f};
    Pht::Color diffuse {0.5f, 0.15f, 0.15f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mRedMaterial = std::make_unique<Pht::Material>(mEnvMapTextureFilenames,
                                                   ambient,
                                                   diffuse,
                                                   specular,
                                                   shininess,
                                                   0.6f);
}

void Materials::CreateGreenMaterial() {
    Pht::Color ambient {0.11f, 0.63f, 0.11f};
    Pht::Color diffuse {0.1f, 0.595f, 0.1f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mGreenMaterial = std::make_unique<Pht::Material>(mEnvMapTextureFilenames,
                                                     ambient,
                                                     diffuse,
                                                     specular,
                                                     shininess,
                                                     reflectivity);
    mGreenMaterial->GetDepthState().mDepthTestAllowedOverride = true;
}

void Materials::CreateLightGrayMaterial() {
    Pht::Color ambient {0.575f, 0.575f, 0.575f};
    Pht::Color diffuse {0.575f, 0.575f, 0.575f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {25.0f};
    mLightGrayMaterial = std::make_unique<Pht::Material>(mEnvMapTextureFilenames,
                                                         ambient,
                                                         diffuse,
                                                         specular,
                                                         shininess,
                                                         reflectivity);
}

void Materials::CreateGrayYellowMaterial() {
    Pht::Color ambient {0.64f, 0.605f, 0.525f};
    Pht::Color diffuse {0.64f, 0.605f, 0.525f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {25.0f};
    mGrayYellowMaterial = std::make_unique<Pht::Material>(mEnvMapTextureFilenames,
                                                          ambient,
                                                          diffuse,
                                                          specular,
                                                          shininess,
                                                          reflectivity);
}

void Materials::CreateRedFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.7325f, 0.255f, 0.255f};
    Pht::Color diffuse {0.36f, 0.235f, 0.255f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mRedFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                             ambient,
                                                             diffuse,
                                                             specular,
                                                             shininess,
                                                             0.37f);
    mRedFieldBlockMaterial->GetDepthState().mDepthTestAllowedOverride = true;
}

void Materials::CreateBlueFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.0f, 0.494f, 0.985f};
    Pht::Color diffuse {0.0f, 0.475f, 0.99f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mBlueFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                              ambient,
                                                              diffuse,
                                                              specular,
                                                              shininess,
                                                              fieldBlockReflectivity);
    mBlueFieldBlockMaterial->GetDepthState().mDepthTestAllowedOverride = true;
}

void Materials::CreateGreenFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.15f, 0.615f, 0.15f};
    Pht::Color diffuse {0.15f, 0.53f, 0.05f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mGreenFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                               ambient,
                                                               diffuse,
                                                               specular,
                                                               shininess,
                                                               fieldBlockReflectivity);
    mGreenFieldBlockMaterial->GetDepthState().mDepthTestAllowedOverride = true;
}

void Materials::CreateGrayFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.5175f, 0.5175f, 0.47175f};
    Pht::Color diffuse {0.495f, 0.495f, 0.45f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {30.0f};
    mGrayFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                              ambient,
                                                              diffuse,
                                                              specular,
                                                              shininess,
                                                              fieldBlockReflectivity);
    mGrayFieldBlockMaterial->GetDepthState().mDepthTestAllowedOverride = true;
}

void Materials::CreateYellowFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.99f, 0.64f, 0.0f};
    Pht::Color diffuse {0.96f, 0.62f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mYellowFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                                ambient,
                                                                diffuse,
                                                                specular,
                                                                shininess,
                                                                fieldBlockReflectivity);
    mYellowFieldBlockMaterial->GetDepthState().mDepthTestAllowedOverride = true;
}
