#include "Materials.hpp"

using namespace BlocksGame;

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
    CreateYellowMaterial(envMapTextures);
    
    Pht::EnvMapTextureFilenames fieldBlockEnvMapTextures {
        "sky_upside_down.jpg",
        "sky_upside_down.jpg",
        "sky_upside_down.jpg",
        "sky_upside_down.jpg",
        "sky_upside_down_patched_mirrored.jpg",
        "sky_upside_down.jpg"
    };
    
    CreateGoldFieldBlockMaterial(fieldBlockEnvMapTextures);
    CreateRedFieldBlockMaterial(fieldBlockEnvMapTextures);
    CreateBlueFieldBlockMaterial(fieldBlockEnvMapTextures);
    CreateGreenFieldBlockMaterial(fieldBlockEnvMapTextures);
    CreateGrayFieldBlockMaterial(fieldBlockEnvMapTextures);
    CreateYellowFieldBlockMaterial(fieldBlockEnvMapTextures);
}

void Materials::CreateGoldMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {1.0f, 0.5f, 0.0f};
    Pht::Color diffuse {1.0f, 0.5f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mGoldMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    reflectivity);
}

void Materials::CreateBlueMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.0f, 0.475f, 1.0f};
    Pht::Color diffuse {0.0f, 0.475f, 1.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mBlueMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    reflectivity);
}

void Materials::CreateRedMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {1.0f, 0.185f, 0.185f};
    Pht::Color diffuse {1.0f, 0.185f, 0.185f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mRedMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                   ambient,
                                                   diffuse,
                                                   specular,
                                                   shininess,
                                                   reflectivity);
}

void Materials::CreateGreenMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.1f, 0.595f, 0.1f};
    Pht::Color diffuse {0.1f, 0.595f, 0.1f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mGreenMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                     ambient,
                                                     diffuse,
                                                     specular,
                                                     shininess,
                                                     reflectivity);
}

void Materials::CreateLightGrayMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.55f, 0.55f, 0.55f};
    Pht::Color diffuse {0.55f, 0.55f, 0.55f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mLightGrayMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                         ambient,
                                                         diffuse,
                                                         specular,
                                                         shininess,
                                                         reflectivity);
}

void Materials::CreateYellowMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.96f, 0.62f, 0.0f};
    Pht::Color diffuse {0.96f, 0.62f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mYellowMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                      ambient,
                                                      diffuse,
                                                      specular,
                                                      shininess,
                                                      reflectivity);
}

void Materials::CreateGoldFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {1.0f, 0.5f, 0.0f};
    Pht::Color diffuse {1.0f, 0.5f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mGoldFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                              ambient,
                                                              diffuse,
                                                              specular,
                                                              shininess,
                                                              fieldBlockReflectivity);
}

void Materials::CreateRedFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {1.0f, 0.185f, 0.185f};
    Pht::Color diffuse {1.0f, 0.185f, 0.185f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mRedFieldBlockMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                             ambient,
                                                             diffuse,
                                                             specular,
                                                             shininess,
                                                             fieldBlockReflectivity);
}

void Materials::CreateBlueFieldBlockMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.0f, 0.475f, 1.0f};
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
    Pht::Color ambient {0.1f, 0.595f, 0.1f};
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
    Pht::Color ambient {0.5f, 0.5f, 0.45f};
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
    Pht::Color ambient {0.96f, 0.62f, 0.0f};
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
