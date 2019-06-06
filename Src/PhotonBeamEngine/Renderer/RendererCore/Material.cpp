#include "Material.hpp"

#include <assert.h>

#include "IImage.hpp"

using namespace Pht;

uint32_t Material::mIdCounter = 0;

Material::Material() :
    mShaderId {ShaderId::VertexColor} {

    mDepthState.mDepthWrite = false;
}

Material::Material(const std::string& textureName) :
    mShaderId {ShaderId::Textured},
    mTexture {TextureCache::GetTexture(textureName, GenerateMipmap::Yes)} {}

Material::Material(const IImage& image, GenerateMipmap generateMipmap) :
    mShaderId {ShaderId::Textured},
    mTexture {TextureCache::InitTexture(image, generateMipmap)} {}

Material::Material(const std::string& textureName,
                   float ambient,
                   float diffuse,
                   float specular,
                   float shininess) :
    mAmbient {Color{ambient, ambient, ambient}},
    mDiffuse {Color{diffuse, diffuse, diffuse}},
    mSpecular {Color{specular, specular, specular}},
    mShininess {shininess},
    mShaderId {ShaderId::TexturedLighting},
    mTexture {TextureCache::GetTexture(textureName, GenerateMipmap::Yes)} {}

Material::Material(const std::string& textureName,
                   const std::string& emissionTextureName,
                   float ambient,
                   float diffuse,
                   float specular,
                   float emissive,
                   float shininess) :
    mAmbient {Color{ambient, ambient, ambient}},
    mDiffuse {Color{diffuse, diffuse, diffuse}},
    mSpecular {Color{specular, specular, specular}},
    mEmissive {Color{emissive, emissive, emissive}},
    mShininess {shininess},
    mShaderId {ShaderId::TexturedEmissiveLighting},
    mTexture {TextureCache::GetTexture(textureName, GenerateMipmap::Yes)},
    mEmissionTexture {TextureCache::GetTexture(emissionTextureName, GenerateMipmap::Yes)} {}

Material::Material(const EnvMapTextureFilenames& envMapTextures,
                   const Color& ambient,
                   const Color& diffuse,
                   const Color& specular,
                   float shininess,
                   float reflectivity) :
    mAmbient {ambient},
    mDiffuse {diffuse},
    mSpecular {specular},
    mShininess {shininess},
    mReflectivity {reflectivity},
    mShaderId {ShaderId::EnvMap},
    mEnvMapTexture {TextureCache::GetTexture(envMapTextures)} {}

Material::Material(const std::string& textureName,
                   const EnvMapTextureFilenames& envMapTextures,
                   const Color& ambient,
                   const Color& diffuse,
                   const Color& specular,
                   float shininess,
                   float reflectivity) :
    mAmbient {ambient},
    mDiffuse {diffuse},
    mSpecular {specular},
    mShininess {shininess},
    mReflectivity {reflectivity},
    mShaderId {ShaderId::TexturedEnvMapLighting},
    mTexture {TextureCache::GetTexture(textureName, GenerateMipmap::Yes)},
    mEnvMapTexture {TextureCache::GetTexture(envMapTextures)} {}

Material::Material(const Color& ambient, 
                   const Color& diffuse, 
                   const Color& specular, 
                   float shininess) : 
    mAmbient {ambient},
    mDiffuse {diffuse},
    mSpecular {specular},
    mShininess {shininess} {}

Material::Material(const Color& color) :
    Material {color, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0f} {}

Blend Material::GetBlend() const {
    if (mBlend == Blend::Additive) {
        return Blend::Additive;
    }

    if (mBlend == Blend::Yes || mOpacity != 1.0f || mShaderId == ShaderId::VertexColor) {
        return Blend::Yes;
    }
    
    return Blend::No;
}

const Texture* Material::GetTexture() const {
    return mTexture.get();
}

const Texture* Material::GetEmissionTexture() const {
    return mEmissionTexture.get();
}

const Texture* Material::GetEnvMapTexture() const {
    return mEnvMapTexture.get();
}

void Material::SetOpacity(float opacity) {
    mOpacity = opacity;
    
    if (mOpacity != 1.0f) {
        mDepthState.mDepthWrite = false;
    }
}

void Material::SetShaderId(ShaderId shaderId) {
    assert(shaderId != ShaderId::Other);
    mShaderId = shaderId;
    
    if (mShaderId == ShaderId::VertexColor) {
        mDepthState.mDepthWrite = false;
    }
}

void Material::SetBlend(Blend blend) {
    mBlend = blend;
    
    switch (blend) {
        case Blend::Yes:
        case Blend::Additive:
            mDepthState.mDepthWrite = false;
            break;
        case Blend::No:
            break;
    }
}
