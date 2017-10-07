#include "Materials.hpp"

using namespace BlocksGame;

Materials::Materials(Pht::IEngine& engine) {
    std::string envMapTexture {"sky_upside_down.jpg"};
    Pht::CubeMapTextures cubeMapTextures {
        envMapTexture,
        envMapTexture,
        envMapTexture,
        envMapTexture,
        "sky_upside_down_patched_mirrored.jpg",
        envMapTexture
    };

    CreateGoldMaterial(cubeMapTextures);
    CreateBlueMaterial(cubeMapTextures);
    CreateRedMaterial(cubeMapTextures);
    CreateGreenMaterial(cubeMapTextures);
    CreateGrayMaterial(cubeMapTextures);
    CreateDarkGrayMaterial(cubeMapTextures);
    CreateLightGrayMaterial(cubeMapTextures);
    CreateYellowMaterial(cubeMapTextures);
    CreateSkyMaterial();
}

void Materials::CreateGoldMaterial(const Pht::CubeMapTextures& cubeMapTextures) {
    Pht::Color ambient {1.0f, 0.5f, 0.0f};
    Pht::Color diffuse {1.0f, 0.5f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    auto reflectivity {0.95f};
    mGoldMaterial = std::make_unique<Pht::Material>(cubeMapTextures,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    reflectivity);
}

void Materials::CreateBlueMaterial(const Pht::CubeMapTextures& cubeMapTextures) {
    Pht::Color ambient {0.0f, 0.5f, 1.0f};
    Pht::Color diffuse {0.0f, 0.5f, 1.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    auto reflectivity {1.0f};
    mBlueMaterial = std::make_unique<Pht::Material>(cubeMapTextures,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    reflectivity);
}

void Materials::CreateRedMaterial(const Pht::CubeMapTextures& cubeMapTextures) {
    Pht::Color ambient {1.0f, 0.2f, 0.2f};
    Pht::Color diffuse {1.0f, 0.2f, 0.2f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    auto reflectivity {0.9f};
    mRedMaterial = std::make_unique<Pht::Material>(cubeMapTextures,
                                                   ambient,
                                                   diffuse,
                                                   specular,
                                                   shininess,
                                                   reflectivity);
}

void Materials::CreateGreenMaterial(const Pht::CubeMapTextures& cubeMapTextures) {
    Pht::Color ambient {0.1f, 0.60f, 0.1f};
    Pht::Color diffuse {0.1f, 0.60f, 0.1f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    auto reflectivity {0.95f};
    mGreenMaterial = std::make_unique<Pht::Material>(cubeMapTextures,
                                                     ambient,
                                                     diffuse,
                                                     specular,
                                                     shininess,
                                                     reflectivity);
}

void Materials::CreateGrayMaterial(const Pht::CubeMapTextures& cubeMapTextures) {
    Pht::Color ambient {0.5f, 0.5f, 0.45f};
    Pht::Color diffuse {0.5f, 0.5f, 0.45f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {30.0f};
    auto reflectivity {1.0f};
    mGrayMaterial = std::make_unique<Pht::Material>(cubeMapTextures,
                                                    ambient,
                                                    diffuse,
                                                    specular,
                                                    shininess,
                                                    reflectivity);
}

void Materials::CreateDarkGrayMaterial(const Pht::CubeMapTextures& cubeMapTextures) {
    Pht::Color ambient {0.4f, 0.4f, 0.4f};
    Pht::Color diffuse {0.4f, 0.4f, 0.4f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    auto reflectivity {1.0f};
    mDarkGrayMaterial = std::make_unique<Pht::Material>(cubeMapTextures,
                                                        ambient,
                                                        diffuse,
                                                        specular,
                                                        shininess,
                                                        reflectivity);
}

void Materials::CreateLightGrayMaterial(const Pht::CubeMapTextures& cubeMapTextures) {
    Pht::Color ambient {0.55f, 0.55f, 0.55f};
    Pht::Color diffuse {0.55f, 0.55f, 0.55f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    auto reflectivity {1.0f};
    mLightGrayMaterial = std::make_unique<Pht::Material>(cubeMapTextures,
                                                         ambient,
                                                         diffuse,
                                                         specular,
                                                         shininess,
                                                         reflectivity);
}

void Materials::CreateYellowMaterial(const Pht::CubeMapTextures& cubeMapTextures) {
    Pht::Color ambient {0.96f, 0.62f, 0.0f};
    Pht::Color diffuse {0.96f, 0.62f, 0.0f};
    Pht::Color specular {1.0f, 1.0f, 1.0f};
    auto shininess {20.0f};
    auto reflectivity {0.95f};
    mYellowMaterial = std::make_unique<Pht::Material>(cubeMapTextures,
                                                      ambient,
                                                      diffuse,
                                                      specular,
                                                      shininess,
                                                      reflectivity);
}

void Materials::CreateSkyMaterial() {
    mSkyMaterial = std::make_unique<Pht::Material>("sky_2.jpg");
}
