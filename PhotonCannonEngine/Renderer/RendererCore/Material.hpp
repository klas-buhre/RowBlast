#ifndef Material_hpp
#define Material_hpp

#include "TextureCache.hpp"

namespace Pht {
    struct Color {
        float mRed {0.0f};
        float mGreen {0.0f};
        float mBlue {0.0f};
        
        Color operator+(const Color& other) const {
            return Color {mRed + other.mRed, mGreen + other.mGreen, mBlue + other.mBlue};
        }
    };
    
    enum class ShaderType {
        PixelLighting,
        VertexLighting,
        TexturedLighting,
        Textured,
        EnvMap,
        VertexColor,
        Particle,
        PointParticle
    };
    
    enum class Blend {
        Yes,
        No
    };

    class Material {
    public:
        Material();
        explicit Material(const std::string& textureName);
        Material(const IImage& image, GenerateMipmap generateMipmap);
        Material(const std::string& textureName,
                 float ambient,
                 float diffuse,
                 float specular,
                 float shininess);
        Material(const EnvMapTextureFilenames& envMapTextureFilenames,
                 const Color& ambient,
                 const Color& diffuse, 
                 const Color& specular, 
                 float shininess,
                 float reflectivity);
        Material(const Color& ambient, 
                 const Color& diffuse, 
                 const Color& specular,
                 float shininess);
        explicit Material(const Color& color);
        
        Blend GetBlend() const;
        GLuint GetTexture() const;

        const Color& GetAmbient() const {
            return mAmbient;
        }
        
        void SetAmbient(const Color& ambient) {
            mAmbient = ambient;
        }

        const Color& GetDiffuse() const {
            return mDiffuse;
        }
        
        void SetDiffuse(const Color& diffuse) {
            mDiffuse = diffuse;
        }

        const Color& GetSpecular() const {
            return mSpecular;
        }
        
        void SetSpecular(const Color& specular) {
            mSpecular = specular;
        }

        void SetBlend(Blend blend) {
            mBlend = blend;
        }
        
        float GetShininess() const {
            return mShininess;
        }
        
        float GetReflectivity() const {
            return mReflectivity;
        }
        
        float GetOpacity() const {
            return mOpacity;
        }
        
        void SetOpacity(float opacity) {
            mOpacity = opacity;
        }
        
        ShaderType GetShaderType() const {
            return mShaderType;
        }
        
        void SetShaderType(ShaderType shaderType) {
            mShaderType = shaderType;
        }

    private:
        Color mAmbient;
        Color mDiffuse;
        Color mSpecular;
        float mShininess {0.0f};
        float mReflectivity {0.0f};
        float mOpacity {1.0f};
        ShaderType mShaderType {ShaderType::VertexLighting};
        std::shared_ptr<Texture> mTexture;
        Blend mBlend {Blend::No};
    };
}

#endif
