#include "Material.hpp"

#include <assert.h>

#include "IImage.hpp"

using namespace Pht;

namespace {
    GLenum ToGlTextureFormat(ImageFormat format) {
        switch (format) {
            case ImageFormat::Gray:
                return GL_LUMINANCE;
            case ImageFormat::GrayAlpha:
                return GL_LUMINANCE_ALPHA;
            case ImageFormat::Rgb:
                return GL_RGB;
            case ImageFormat::Rgba:
                return GL_RGBA;
        }
    }
    
    GLenum ToGlType(int numBits, GLenum format) {
        switch (numBits) {
            case 8:
                return GL_UNSIGNED_BYTE;
            case 4:
                if (format == GL_RGBA) {
                    return GL_UNSIGNED_SHORT_4_4_4_4;
                } else {
                    assert(!"Unsupported format.");
                }
                break;
            default:
                assert(!"Unsupported format.");
        }
    }
    
    void GlTexImage(GLenum target, const IImage& image) {
        auto format {ToGlTextureFormat(image.GetFormat())};
        auto type {ToGlType(image.GetBitsPerComponent(), format)};
        auto* data {image.GetImageData()};
        auto size {image.GetSize()};
        glTexImage2D(target, 0, format, size.x, size.y, 0, format, type, data);
    }
    
    void GlTexImage(GLenum target, const std::string& filename) {
        auto image {Pht::LoadImage(filename)};
        GlTexImage(target, *image);
    }
}

Material::Material() :
    mShaderType {ShaderType::VertexColor} {}

Material::Material(const std::string& textureName) :
    mShaderType {ShaderType::Textured} {

    auto image {Pht::LoadImage(textureName)};
    InitTexture(*image, GenerateMipmap::Yes);
}

Material::Material(const IImage& image, GenerateMipmap GenerateMipmap) :
    mShaderType {ShaderType::Textured} {
    
    InitTexture(image, GenerateMipmap);
}

Material::Material(const std::string& textureName,
                   float ambient,
                   float diffuse,
                   float specular,
                   float shininess) :
    mAmbient {Color{ambient, ambient, ambient}},
    mDiffuse {Color{diffuse, diffuse, diffuse}},
    mSpecular {Color{specular, specular, specular}},
    mShininess {shininess},
    mShaderType {ShaderType::TexturedLighting} {
    
    auto image {Pht::LoadImage(textureName)};
    InitTexture(*image, GenerateMipmap::Yes);
}

Material::Material(const CubeMapTextures& cubeMapTextures,
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
    mShaderType {ShaderType::CubeMap} {

    InitCubeTexures(cubeMapTextures);
}

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

void Material::InitTexture(const IImage& image, GenerateMipmap generateMipmap) {
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    
    if (generateMipmap == GenerateMipmap::Yes) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    GlTexImage(GL_TEXTURE_2D, image);
    
    if (generateMipmap == GenerateMipmap::Yes) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

void Material::InitCubeTexures(const CubeMapTextures& filenames) {
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
    
    GlTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, filenames.mPositiveX);
    GlTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, filenames.mNegativeX);
    GlTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, filenames.mPositiveY);
    GlTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, filenames.mNegativeY);
    GlTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, filenames.mPositiveZ);
    GlTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, filenames.mNegativeZ);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
