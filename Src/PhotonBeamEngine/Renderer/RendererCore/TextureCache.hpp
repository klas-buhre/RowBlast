#ifndef Texture_hpp
#define Texture_hpp

#include <string>
#include <memory>

namespace Pht {
    enum class GenerateMipmap {
        Yes,
        No
    };
    
    struct TextureHandles;
    
    class Texture {
    public:
        Texture(bool hasPremultipliedAlpha);
        ~Texture();
        
        const TextureHandles* GetHandles() const {
            return mHandles.get();
        }

        bool HasPremultipliedAlpha() const {
            return mHasPremultipliedAlpha;
        }
        
    private:
        std::unique_ptr<TextureHandles> mHandles;
        bool mHasPremultipliedAlpha {false};
    };
    
    struct EnvMapTextureFilenames {
        bool operator==(const EnvMapTextureFilenames& other) const;
        
        std::string mPositiveX;
        std::string mNegativeX;
        std::string mPositiveY;
        std::string mNegativeY;
        std::string mPositiveZ;
        std::string mNegativeZ;
    };
    
    class IImage;
    
    namespace TextureCache {
        std::shared_ptr<Texture> GetTexture(const std::string& textureName,
                                            GenerateMipmap generateMipmap);
        std::shared_ptr<Texture> GetTexture(const EnvMapTextureFilenames& filenames);
        std::shared_ptr<Texture> InitTexture(const IImage& image, GenerateMipmap generateMipmap);
    }
}

#endif
