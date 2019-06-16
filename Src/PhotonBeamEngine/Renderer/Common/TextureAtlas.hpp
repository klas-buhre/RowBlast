#ifndef TextureAtlas_hpp
#define TextureAtlas_hpp

#include <vector>

#include "Vector.hpp"
#include "Optional.hpp"

namespace Pht {
    class IImage;
    
    enum class TexturePackingDirection {
        Horizontal,
        Vertical
    };
    
    using SubImages = std::vector<std::unique_ptr<const IImage>>;
    
    class TextureAtlas {
    public:
        std::unique_ptr<IImage> CreateAtlasImage(const SubImages& images,
                                                 TexturePackingDirection packingDirection);
        
        struct SubTextureUVs {
            Vec2 mBottomLeft;
            Vec2 mBottomRight;
            Vec2 mTopRight;
            Vec2 mTopLeft;
        };
        
        const SubTextureUVs* GetSubTextureUVs(int subTextureIndex) const;
        
    private:
        struct SubTexture {
            IVec2 mPosition;
            const IImage* mImage;
        };
        
        Optional<IVec2> PackTextures(std::vector<SubTexture>& subTextures,
                                     TexturePackingDirection packingDirection);
        Optional<IVec2> PackTexturesHorizontally(std::vector<SubTexture>& subTextures);
        Optional<IVec2> PackTexturesVertically(std::vector<SubTexture>& subTextures);
        bool PackVertically(std::vector<SubTexture>& subTextures, const IVec2& size);
        std::unique_ptr<IImage> CreateAtlasImage(const std::vector<SubTexture>& subTextures,
                                                 const IVec2& atlasImageSize);
        
        std::vector<SubTextureUVs> mSubTextureUVs;
    };
}

#endif
