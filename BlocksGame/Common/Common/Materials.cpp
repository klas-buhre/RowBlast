#include "Materials.hpp"

using namespace BlocksGame;

namespace {
    const auto reflectivity {0.825f};
}

Materials::Materials(Pht::IEngine& engine) {
    std::string envMapTexture {"sky_upside_down.jpg"};
    Pht::EnvMapTextureFilenames envMapTextures {
        envMapTexture,
        envMapTexture,
        envMapTexture,
        envMapTexture,
        "sky_upside_down_patched_mirrored.jpg",
        envMapTexture
    };

    CreateGoldMaterial(envMapTextures);
    CreateBlueMaterial(envMapTextures);
    CreateRedMaterial(envMapTextures);
    CreateGreenMaterial(envMapTextures);
    CreateGrayMaterial(envMapTextures);
    CreateDarkGrayMaterial(envMapTextures);
    CreateLightGrayMaterial(envMapTextures);
    CreateYellowMaterial(envMapTextures);
    CreateSkyMaterial();
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

void Materials::CreateGrayMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.5f, 0.5f, 0.45f};
    Pht::Color diffuse {0.5f, 0.5f, 0.45f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {30.0f};
    mGrayMaterial = std::make_unique<Pht::Material>(envMapTextures,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    reflectivity);
}

void Materials::CreateDarkGrayMaterial(const Pht::EnvMapTextureFilenames& envMapTextures) {
    Pht::Color ambient {0.4f, 0.4f, 0.4f};
    Pht::Color diffuse {0.4f, 0.4f, 0.4f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    mDarkGrayMaterial = std::make_unique<Pht::Material>(envMapTextures,
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

void Materials::CreateSkyMaterial() {
    mSkyMaterial = std::make_unique<Pht::Material>("sky_2.jpg");
}
