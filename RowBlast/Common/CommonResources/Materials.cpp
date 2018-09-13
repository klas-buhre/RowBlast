#include "Materials.hpp"

using namespace RowBlast;

namespace {
    const auto reflectivity {0.84f};
    const auto fieldBlockReflectivity {0.825f};
}

Materials::Materials(Pht::IEngine& engine) {
    Pht::EnvMapTextureFilenames envMapTextures {
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg",
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg",
        "cloud_B_envmap.jpg",
        "cloud_C_envmap.jpg"
    };

    CreateGoldMaterial(envMapTextures);
    CreateBlueMaterial(envMapTextures);
    CreateRedMaterial(envMapTextures);
    CreateGreenMaterial(envMapTextures);
    CreateLightGrayMaterial(envMapTextures);
    
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

void Materials::CreateGoldMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.91f, 0.55f, 0.08f};
    Pht::Color diffuse {0.51f, 0.4f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mGoldMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    0.6f);
}

void Materials::CreateBlueMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.05f, 0.5225f, 1.0f};
    Pht::Color diffuse {0.0f, 0.475f, 1.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mBlueMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    reflectivity);
}

void Materials::CreateRedMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.9f, 0.42f, 0.42f};
    Pht::Color diffuse {0.5f, 0.15f, 0.15f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mRedMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                   ambient,
                                                   diffuse,
                                                   specular,
                                                   shininess,
                                                   0.6f);
}

void Materials::CreateGreenMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.11f, 0.63f, 0.11f};
    Pht::Color diffuse {0.1f, 0.595f, 0.1f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {21.0f};
    mGreenMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                     ambient,
                                                     diffuse,
                                                     specular,
                                                     shininess,
                                                     reflectivity);
}

void Materials::CreateLightGrayMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.575f, 0.575f, 0.575f};
    Pht::Color diffuse {0.575f, 0.575f, 0.575f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {25.0f};
    mLightGrayMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                         ambient,
                                                         diffuse,
                                                         specular,
                                                         shininess,
                                                         reflectivity);
}

void Materials::CreateRedFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.795f, 0.285f, 0.265f};
    Pht::Color diffuse {0.5f, 0.20f, 0.18f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mRedFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                             ambient,
                                                             diffuse,
                                                             specular,
                                                             shininess,
                                                             0.5f);
}

void Materials::CreateBlueFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.0f, 0.494f, 1.04f};
    Pht::Color diffuse {0.0f, 0.475f, 1.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mBlueFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                              ambient,
                                                              diffuse,
                                                              specular,
                                                              shininess,
                                                              fieldBlockReflectivity);
}

void Materials::CreateGreenFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.1f, 0.615f, 0.1f};
    Pht::Color diffuse {0.1f, 0.595f, 0.1f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mGreenFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                               ambient,
                                                               diffuse,
                                                               specular,
                                                               shininess,
                                                               fieldBlockReflectivity);
}

void Materials::CreateGrayFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.515f, 0.515, 0.4635f};
    Pht::Color diffuse {0.5f, 0.5f, 0.45f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {30.0f};
    mGrayFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                              ambient,
                                                              diffuse,
                                                              specular,
                                                              shininess,
                                                              fieldBlockReflectivity);
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
}
