#include "Material.hpp"

#include <assert.h>

#include "IImage.hpp"

using namespace Pht;

Material::Material() :
    mShaderType {ShaderType::VertexColor} {}

Material::Material(const std::string& textureName) :
    mShaderType {ShaderType::Textured},
    mTexture {TextureCache::GetTexture(textureName, GenerateMipmap::Yes)} {}

Material::Material(const IImage& image, GenerateMipmap generateMipmap) :
    mShaderType {ShaderType::Textured},
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
    mShaderType {ShaderType::TexturedLighting},
    mTexture {TextureCache::GetTexture(textureName, GenerateMipmap::Yes)} {}

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
    mShaderType {ShaderType::EnvMap},
    mTexture {TextureCache::GetTexture(envMapTextures)} {}

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
    if (mBlend == Blend::Yes || mOpacity != 1.0f || mShaderType == ShaderType::VertexColor) {
        return Blend::Yes;
    }
    
    return Blend::No;
}

const Texture* Material::GetTexture() const {
    return mTexture.get();
}

void Material::SetOpacity(float opacity) {
    mOpacity = opacity;
    
    if (mOpacity != 1.0f) {
        mDepthState.mDepthWrite = false;
    }
}

void Material::SetShaderType(ShaderType shaderType) {
    mShaderType = shaderType;
    
    if (mShaderType == ShaderType::VertexColor) {
        mDepthState.mDepthWrite = false;
    }
}

void Material::SetBlend(Blend blend) {
    mBlend = blend;
    
    if (mBlend == Blend::Yes) {
        mDepthState.mDepthWrite = false;
    }
}
