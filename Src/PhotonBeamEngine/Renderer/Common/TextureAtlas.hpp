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

    struct SubTextureUVs {
        Vec2 mBottomLeft;
        Vec2 mBottomRight;
        Vec2 mTopRight;
        Vec2 mTopLeft;
    };

    class TextureAtlas {
    public:
        std::unique_ptr<IImage> CreateAtlasImage(const SubImages& images,
                                                 TexturePackingDirection packingDirection);
        
        const SubTextureUVs* GetSubTextureUVs(int subTextureIndex) const;
        
    private:
        std::vector<SubTextureUVs> mSubTextureUVs;
    };
}

#endif
