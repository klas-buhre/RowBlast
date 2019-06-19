#include "TextureCache.hpp"

#include <vector>
#include <mutex>
#include <algorithm>
#include <assert.h>

#include <OpenGLES/ES3/gl.h>

#include "IImage.hpp"
#include "TextureAtlas.hpp"
#include "GLES3Handles.hpp"

using namespace Pht;

namespace {
    struct TwoDTextureKey {
        std::string mFilename;
        GenerateMipmap mGenerateMipmap;
    };

    static std::mutex mutex;
    static std::vector<std::pair<TwoDTextureKey, std::weak_ptr<Texture>>> twoDTextures;
    static std::vector<std::pair<EnvMapTextureFilenames, std::weak_ptr<Texture>>> envMapTextures;
    
    GLenum ToGLTextureFormat(ImageFormat format) {
        switch (format) {
            case ImageFormat::Rgb:
                return GL_RGB;
            case ImageFormat::Rgba:
                return GL_RGBA;
            case ImageFormat::Gray:
                return GL_LUMINANCE;
            case ImageFormat::GrayAlpha:
                return GL_LUMINANCE_ALPHA;
            case ImageFormat::Alpha:
                return GL_ALPHA;
        }
    }
    
    GLenum ToGLType(int numBits, GLenum format) {
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
    
    void GLTexImage(GLenum target, const IImage& image) {
        auto format = ToGLTextureFormat(image.GetFormat());
        auto type = ToGLType(image.GetBitsPerComponent(), format);
        auto* data = image.GetImageData();
        auto size = image.GetSize();
        glTexImage2D(target, 0, format, size.x, size.y, 0, format, type, data);
    }
    
    void GLTexImage(GLenum target, const std::string& filename) {
        auto image = Pht::LoadImage(filename);
        GLTexImage(target, *image);
    }
    
    std::shared_ptr<Texture>
    CreateAtlasTextureNoMipmap(const IImage& image,
                               std::unique_ptr<TextureAtlas> textureAtlas) {
        if (image.GetFormat() == ImageFormat::Alpha) {
            // Disable byte alignment restriction.
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        
        auto texture = std::make_shared<Texture>(image.HasPremultipliedAlpha(),
                                                 std::move(textureAtlas));
        glBindTexture(GL_TEXTURE_2D, texture->GetHandles()->mGLHandle);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        GLTexImage(GL_TEXTURE_2D, image);

        glBindTexture(GL_TEXTURE_2D, 0);
        
        if (image.GetFormat() == ImageFormat::Alpha) {
            // Restore byte-alignment.
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }

        return texture;
    }

    std::shared_ptr<Texture> CreateTexture(const IImage& image,
                                           GenerateMipmap generateMipmap,
                                           std::unique_ptr<TextureAtlas> textureAtlas) {
        auto texture = std::make_shared<Texture>(image.HasPremultipliedAlpha(),
                                                 std::move(textureAtlas));
        
        glBindTexture(GL_TEXTURE_2D, texture->GetHandles()->mGLHandle);
        
        if (generateMipmap == GenerateMipmap::Yes) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        GLTexImage(GL_TEXTURE_2D, image);
        
        if (generateMipmap == GenerateMipmap::Yes) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        
        return texture;
    }
    
    std::shared_ptr<Texture> CreateEnvMapTexture(const EnvMapTextureFilenames& filenames) {
        auto texture = std::make_shared<Texture>(false, nullptr);
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture->GetHandles()->mGLHandle);
        
        GLTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, filenames.mPositiveX);
        GLTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, filenames.mNegativeX);
        GLTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, filenames.mPositiveY);
        GLTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, filenames.mNegativeY);
        GLTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, filenames.mPositiveZ);
        GLTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, filenames.mNegativeZ);
        
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

Texture::Texture(bool hasPremultipliedAlpha, std::unique_ptr<TextureAtlas> atlas) :
    mHandles {std::make_unique<TextureHandles>()},
    mAtlas {std::move(atlas)},
    mHasPremultipliedAlpha {hasPremultipliedAlpha} {
    
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &mHandles->mGLHandle);
}

Texture::~Texture() {
    glDeleteTextures(1, &mHandles->mGLHandle);
}

std::shared_ptr<Texture> TextureCache::GetTexture(const std::string& textureName,
                                                  GenerateMipmap generateMipmap) {
    if (textureName.empty()) {
        return nullptr;
    }

    std::lock_guard<std::mutex> guard {mutex};
    
    twoDTextures.erase(
        std::remove_if(
            std::begin(twoDTextures),
            std::end(twoDTextures),
            [] (const auto& entry) { return entry.second.lock() == std::shared_ptr<Texture>(); }),
        std::end(twoDTextures));
    
    for (const auto& entry: twoDTextures) {
        if (auto texture = entry.second.lock()) {
            auto& key = entry.first;
            if (textureName == key.mFilename && generateMipmap == key.mGenerateMipmap) {
                return texture;
            }
        }
    }
    
    auto image = Pht::LoadImage(textureName);
    auto texture = CreateTexture(*image, generateMipmap, nullptr);
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
        if (auto texture = entry.second.lock()) {
            auto& key = entry.first;
            if (filenames == key) {
                return texture;
            }
        }
    }

    auto texture = CreateEnvMapTexture(filenames);
    envMapTextures.emplace_back(filenames, texture);
    return texture;
}

std::shared_ptr<Texture> TextureCache::InitTexture(const IImage& image,
                                                   GenerateMipmap generateMipmap) {
    return CreateTexture(image, generateMipmap, nullptr);
}

std::shared_ptr<Texture>
TextureCache::GetTextureAtlas(const std::vector<std::string>& filenames,
                              const TextureAtlasConfig& textureAtlasConfig) {
    return nullptr;
}

std::shared_ptr<Texture>
TextureCache::InitTextureAtlas(const std::vector<std::unique_ptr<const IImage>>& images,
                               const TextureAtlasConfig& textureAtlasConfig) {
    auto textureAtlas = std::make_unique<TextureAtlas>();
    auto atlasImage = textureAtlas->CreateAtlasImage(images, textureAtlasConfig);
    auto texture = textureAtlasConfig.mGenerateMipmap == GenerateMipmap::Yes ?
                   CreateTexture(*atlasImage, GenerateMipmap::Yes, std::move(textureAtlas)) :
                   CreateAtlasTextureNoMipmap(*atlasImage, std::move(textureAtlas));
    return texture;
}
