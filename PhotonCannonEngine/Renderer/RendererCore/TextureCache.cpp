#include "TextureCache.hpp"

#include <vector>
#include <mutex>
#include <algorithm>
#include <assert.h>

#include "IImage.hpp"

using namespace Pht;

namespace {
    struct TwoDTexture: public Texture {
        std::string mFilename;
        GenerateMipmap mGenerateMipmap;
    };

    struct CubeTexture: public Texture {
        CubeMapTextures mFilenames;
    };
    
    static std::mutex mutex;
    static std::vector<std::weak_ptr<TwoDTexture>> twoDTextures;
    static std::vector<std::weak_ptr<CubeTexture>> cubeDTextures;
}

Texture::~Texture() {
    glDeleteTextures(1, &mHandle);
}

std::shared_ptr<Texture> TextureCache::GetTexture(const std::string& textureName,
                                                  GenerateMipmap generateMipmap) {
    std::lock_guard<std::mutex> guard(mutex);
    
    twoDTextures.erase(
        std::remove_if(
            std::begin(twoDTextures),
            std::end(twoDTextures),
            [] (const auto& texture) { return texture.lock() == std::shared_ptr<TwoDTexture>(); }),
        std::end(twoDTextures));
    
    for (const auto& texturePtr: twoDTextures) {
        if (auto texture {texturePtr.lock()}) {
            if (textureName == texture->mFilename && generateMipmap == texture->mGenerateMipmap) {
                return texture;
            }
        }
    }
    
    auto image {Pht::LoadImage(textureName)};
    // auto textureHandle {InitTexture(*image, generateMipmap)};
    
    return nullptr;
}
