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
        TexturedEmissiveLighting,
        Textured,
        EnvMap,
        VertexColor,
        Particle,
        ParticleTextureColor,
        ParticleNoAlphaTexture,
        PointParticle
    };
    
    enum class Blend {
        Yes,
        No
    };
    
    struct DepthState {
        bool mDepthTest {true};
        bool mDepthTestAllowedOverride {false};
        bool mDepthWrite {true};
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
        Material(const std::string& textureName,
                 const std::string& emissionTextureName,
                 float ambient,
                 float diffuse,
                 float specular,
                 float emissive,
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
        const Texture* GetTexture() const;
        const Texture* GetEmissionTexture() const;
        void SetOpacity(float opacity);
        void SetShaderType(ShaderType shaderType);
        void SetBlend(Blend blend);

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

        const Color& GetEmissive() const {
            return mEmissive;
        }
        
        void SetEmissive(const Color& emissive) {
            mEmissive = emissive;
        }

        float GetShininess() const {
            return mShininess;
        }
        
        float GetReflectivity() const {
            return mReflectivity;
        }
        
        void SetReflectivity(float reflectivity) {
            mReflectivity = reflectivity;
        }
        
        float GetOpacity() const {
            return mOpacity;
        }
        
        ShaderType GetShaderType() const {
            return mShaderType;
        }
        
        DepthState& GetDepthState() {
            return mDepthState;
        }

        const DepthState& GetDepthState() const {
            return mDepthState;
        }

    private:
        Color mAmbient;
        Color mDiffuse;
        Color mSpecular;
        Color mEmissive;
        float mShininess {0.0f};
        float mReflectivity {0.0f};
        float mOpacity {1.0f};
        ShaderType mShaderType {ShaderType::VertexLighting};
        std::shared_ptr<Texture> mTexture;
        std::shared_ptr<Texture> mEmissionTexture;
        Blend mBlend {Blend::No};
        DepthState mDepthState;
    };
}

#endif
