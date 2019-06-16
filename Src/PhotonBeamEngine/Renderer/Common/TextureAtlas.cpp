#include "TextureAtlas.hpp"

#include <assert.h>

#include "IImage.hpp"

using namespace Pht;

namespace {
    constexpr auto maxSize = 2048;
    constexpr auto minSize = 32;
    
    bool ImagesAreCompatible(const SubImages& images) {
        const IImage* previousImage = nullptr;
        
        for (auto& image: images) {
            if (previousImage) {
                if (image->GetFormat() != previousImage->GetFormat() ||
                    image->GetBitsPerComponent() != previousImage->GetBitsPerComponent() ||
                    image->HasPremultipliedAlpha() != previousImage->HasPremultipliedAlpha()) {
                    
                    return false;
                }
            }
            
            previousImage = image.get();
        }
        
        return true;
    }
    
    bool TextureFitsInAtlas(const IVec2& subTexturePosition,
                            const IVec2& subTextureSize,
                            const IVec2& atlasSize) {
        return false;
    }
}

std::unique_ptr<IImage> TextureAtlas::CreateAtlasImage(const SubImages& images,
                                                       TexturePackingDirection packingDirection) {
    if (!ImagesAreCompatible(images)) {
        assert(false);
        return nullptr;
    }
    
    std::vector<SubTexture> subTextures;
    for (auto& image: images) {
        subTextures.push_back({.mImage = image.get()});
    }
    
    auto atlasImageSize = PackTextures(subTextures, packingDirection);
    if (atlasImageSize.HasValue()) {
        return CreateAtlasImage(subTextures, atlasImageSize.GetValue());
    }
    
    return nullptr;
}

Optional<IVec2> TextureAtlas::PackTextures(std::vector<SubTexture>& subTextures,
                                           TexturePackingDirection packingDirection) {
    switch (packingDirection) {
        case TexturePackingDirection::Horizontal:
            return PackTexturesHorizontally(subTextures);
        case TexturePackingDirection::Vertical:
            return PackTexturesVertically(subTextures);
    }
}

Optional<IVec2> TextureAtlas::PackTexturesHorizontally(std::vector<SubTexture>& subTextures) {

    return Optional<IVec2> {};
}

Optional<IVec2> TextureAtlas::PackTexturesVertically(std::vector<SubTexture>& subTextures) {
    IVec2 size {minSize, minSize};
    auto numTries = 0;
    
    while (size.x <= maxSize && size.y <= maxSize) {
        if (PackVertically(subTextures, size)) {
            return size;
        }
        
        if (numTries % 2 == 0) {
            size.y = size.y << 1;
        } else {
            size.x = size.x << 1;
        }
        
        ++numTries;
    }

    return Optional<IVec2> {};
}

bool TextureAtlas::PackVertically(std::vector<SubTexture>& subTextures, const IVec2& atlasSize) {
    IVec2 subTexturePosition {0, 0};
    auto xMax = 0;
    
    for (auto& subTexture: subTextures) {
        auto subTextureSize = subTexture.mImage->GetSize();
        if (!TextureFitsInAtlas(subTexturePosition, subTextureSize, atlasSize)) {
            subTexturePosition = {xMax, 0};
            if (!TextureFitsInAtlas(subTexturePosition, subTextureSize, atlasSize)) {
                return false;
            }
        }
        
        subTexture.mPosition = subTexturePosition;
        
        subTexturePosition.y += subTextureSize.y;
        if (subTexturePosition.x + subTextureSize.x > xMax) {
            xMax = subTexturePosition.x + subTextureSize.x;
        }
    }
    
    return true;
}

std::unique_ptr<IImage>
TextureAtlas::CreateAtlasImage(const std::vector<SubTexture>& texturePositions,
                               const IVec2& atlasImageSize) {
    return nullptr;
}
