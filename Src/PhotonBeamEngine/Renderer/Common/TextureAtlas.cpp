#include "TextureAtlas.hpp"

#include <assert.h>

#include "IImage.hpp"

using namespace Pht;

namespace {
    constexpr auto maxSize = 2048;
    constexpr auto minSize = 32;
    
    struct SubTexture {
        IVec2 mPosition;
        const IImage* mImage;
    };
    
    bool ImagesAreCompatible(const SubImages& images) {
        const IImage* previousImage = nullptr;
        
        for (auto& image: images) {
            if (image == nullptr) {
                return false;
            }
            
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
    
    bool TextureFits(const IVec2& subTexturePosition,
                     const IVec2& subTextureSize,
                     const IVec2& atlasSize) {
        return subTexturePosition.x >= 0 && subTexturePosition.x + subTextureSize.x < atlasSize.x &&
               subTexturePosition.y >= 0 && subTexturePosition.y + subTextureSize.y < atlasSize.y;
    }
    
    bool PackVertically(std::vector<SubTexture>& subTextures, const IVec2& atlasSize) {
        IVec2 subTexturePosition {0, 0};
        auto xMax = 0;
        
        for (auto& subTexture: subTextures) {
            auto subTextureSize = subTexture.mImage->GetSize();
            if (!TextureFits(subTexturePosition, subTextureSize, atlasSize)) {
                subTexturePosition = {xMax, 0};
                if (!TextureFits(subTexturePosition, subTextureSize, atlasSize)) {
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
    
    Optional<IVec2> PackTexturesVertically(std::vector<SubTexture>& subTextures) {
        IVec2 atlasImageSize {minSize, minSize};
        auto counter = 1;
        
        while (atlasImageSize.x <= maxSize && atlasImageSize.y <= maxSize) {
            if (PackVertically(subTextures, atlasImageSize)) {
                return atlasImageSize;
            }
            
            if (counter % 2 == 0) {
                atlasImageSize.x *= 2;
            } else {
                atlasImageSize.y *= 2;
            }
            
            ++counter;
        }

        return Optional<IVec2> {};
    }

    bool PackHorizontally(std::vector<SubTexture>& subTextures, const IVec2& atlasSize) {
        IVec2 subTexturePosition {0, 0};
        auto yMax = 0;
        
        for (auto& subTexture: subTextures) {
            auto subTextureSize = subTexture.mImage->GetSize();
            if (!TextureFits(subTexturePosition, subTextureSize, atlasSize)) {
                subTexturePosition = {0, yMax};
                if (!TextureFits(subTexturePosition, subTextureSize, atlasSize)) {
                    return false;
                }
            }
            
            subTexture.mPosition = subTexturePosition;
            
            subTexturePosition.x += subTextureSize.x;
            if (subTexturePosition.y + subTextureSize.y > yMax) {
                yMax = subTexturePosition.y + subTextureSize.y;
            }
        }
        
        return true;
    }

    Optional<IVec2> PackTexturesHorizontally(std::vector<SubTexture>& subTextures) {
        IVec2 atlasImageSize {minSize, minSize};
        auto counter = 1;
        
        while (atlasImageSize.x <= maxSize && atlasImageSize.y <= maxSize) {
            if (PackHorizontally(subTextures, atlasImageSize)) {
                return atlasImageSize;
            }
            
            if (counter % 2 == 0) {
                atlasImageSize.y *= 2;
            } else {
                atlasImageSize.x *= 2;
            }
            
            ++counter;
        }

        return Optional<IVec2> {};
    }

    Optional<IVec2> PackTextures(std::vector<SubTexture>& subTextures,
                                 TexturePackingDirection packingDirection) {
        switch (packingDirection) {
            case TexturePackingDirection::Horizontal: {
                auto atlasImageSize = PackTexturesHorizontally(subTextures);
                if (atlasImageSize.HasValue()) {
                    return atlasImageSize;
                }
                return PackTexturesVertically(subTextures);
            }
            case TexturePackingDirection::Vertical: {
                auto atlasImageSize = PackTexturesVertically(subTextures);
                if (atlasImageSize.HasValue()) {
                    return atlasImageSize;
                }
                return PackTexturesHorizontally(subTextures);
            }
        }
    }

    std::unique_ptr<IImage>
    CreateAtlasImageAndCalcUVs(std::vector<SubTextureUVs> subTextureUVs,
                               const std::vector<SubTexture>& subTexturePositions,
                               const IVec2& atlasImageSize) {
        return nullptr;
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
        return CreateAtlasImageAndCalcUVs(mSubTextureUVs, subTextures, atlasImageSize.GetValue());
    }
    
    return nullptr;
}

const SubTextureUVs* TextureAtlas::GetSubTextureUVs(int subTextureIndex) const {
    if (subTextureIndex < mSubTextureUVs.size()) {
        return &mSubTextureUVs[subTextureIndex];
    }
    
    return nullptr;
}
