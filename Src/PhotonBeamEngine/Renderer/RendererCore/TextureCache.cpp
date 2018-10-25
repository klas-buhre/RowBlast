#include "TextureCache.hpp"

#include <vector>
#include <mutex>
#include <algorithm>
#include <assert.h>

#include "IImage.hpp"

using namespace Pht;

namespace {
    struct TwoDTextureKey {
        std::string mFilename;
        GenerateMipmap mGenerateMipmap;
    };

    static std::mutex mutex;
    static std::vector<std::pair<TwoDTextureKey, std::weak_ptr<Texture>>> twoDTextures;
    static std::vector<std::pair<EnvMapTextureFilenames, std::weak_ptr<Texture>>> envMapTextures;
    
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
    
    std::shared_ptr<Texture> CreateTexture(const IImage& image, GenerateMipmap generateMipmap) {
        auto texture {std::make_shared<Texture>(image.HasPremultipliedAlpha())};
        
        glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
        
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
        
        return texture;
    }
    
    std::shared_ptr<Texture> CreateEnvMapTexture(const EnvMapTextureFilenames& filenames) {
        auto texture {std::make_shared<Texture>(false)};
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture->GetHandle());
        
        GlTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, filenames.mPositiveX);
        GlTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, filenames.mNegativeX);
        GlTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, filenames.mPositiveY);
        GlTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, filenames.mNegativeY);
        GlTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, filenames.mPositiveZ);
        GlTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, filenames.mNegativeZ);
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        
        return texture;
    }
}

bool EnvMapTextureFilenames::operator==(const EnvMapTextureFilenames& other) const {
    return mPositiveX == other.mPositiveX && mNegativeX == other.mNegativeX &&
           mPositiveY == other.mPositiveY && mNegativeY == other.mNegativeY &&
           mPositiveZ == other.mPositiveZ && mNegativeZ == other.mNegativeZ;
}

Texture::Texture(bool hasPremultipliedAlpha) :
    mHasPremultipliedAlpha {hasPremultipliedAlpha} {
    
    glGenTextures(1, &mHandle);
}

Texture::~Texture() {
    glDeleteTextures(1, &mHandle);
}

std::shared_ptr<Texture> TextureCache::GetTexture(const std::string& textureName,
                                                  GenerateMipmap generateMipmap) {
    std::lock_guard<std::mutex> guard {mutex};
    
    twoDTextures.erase(
        std::remove_if(
            std::begin(twoDTextures),
            std::end(twoDTextures),
            [] (const auto& entry) { return entry.second.lock() == std::shared_ptr<Texture>(); }),
        std::end(twoDTextures));
    
    for (const auto& entry: twoDTextures) {
        if (auto texture {entry.second.lock()}) {
            auto& key {entry.first};
            
            if (textureName == key.mFilename && generateMipmap == key.mGenerateMipmap) {
                return texture;
            }
        }
    }
    
    auto image {Pht::LoadImage(textureName)};
    auto texture {CreateTexture(*image, generateMipmap)};
    twoDTextures.emplace_back(TwoDTextureKey{textureName, generateMipmap}, texture);
    return texture;
}

std::shared_ptr<Texture> TextureCache::GetTexture(const EnvMapTextureFilenames& filenames) {
    std::lock_guard<std::mutex> guard {mutex};
    
    envMapTextures.erase(
        std::remove_if(
            std::begin(envMapTextures),
            std::end(envMapTextures),
            [] (const auto& entry) { return entry.second.lock() == std::shared_ptr<Texture>(); }),
        std::end(envMapTextures));
    
    for (const auto& entry: envMapTextures) {
        if (auto texture {entry.second.lock()}) {
            auto& key {entry.first};
            
            if (filenames == key) {
                return texture;
            }
        }
    }

    auto texture {CreateEnvMapTexture(filenames)};
    envMapTextures.emplace_back(filenames, texture);
    return texture;
}

std::shared_ptr<Texture> TextureCache::InitTexture(const IImage& image,
                                                   GenerateMipmap generateMipmap) {
    return CreateTexture(image, generateMipmap);
}
