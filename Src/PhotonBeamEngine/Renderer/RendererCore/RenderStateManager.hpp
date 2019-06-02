#ifndef RenderStateManager_hpp
#define RenderStateManager_hpp

#include <OpenGLES/ES3/gl.h>

namespace Pht {
    class ShaderProgram;
    class Material;
    class Vbo;
    
    class RenderStateManager {
    public:
        void Init();
        void SetBlend(bool enabled);
        void SetBlendFunc(GLenum sFactor, GLenum dFactor);
        void SetDepthTest(bool enabled);
        void SetDepthWrite(bool enabled);
        void SetCullFace(bool enabled);
        void SetScissorTest(bool enabled);
        void UseShader(ShaderProgram& shaderProgram);
        void InvalidateShader();
        
        void UseMaterial(const Material& material) {
            mMaterial = &material;
        }

        void UseVbo(const Vbo& vbo) {
            mVbo = &vbo;
        }

        bool IsShaderInUse(const ShaderProgram& shaderProgram) const {
            return &shaderProgram == mShaderProgram;
        }
        
        bool IsMaterialInUse(const Material& material) const {
            return &material == mMaterial;
        }

        bool IsVboInUse(Vbo& vbo) const {
            return &vbo == mVbo;
        }

    private:
        void UpdateGlBlend();
        void UpdateGlBlendFunc();
        void UpdateGlDepthTest();
        void UpdateGlDepthWrite();
        void UpdateGlCullFace();
        void UpdateGlScissorTest();

        bool mBlendEnabled {false};
        GLenum mBlendSFactor {GL_ONE};
        GLenum mBlendDFactor {GL_ZERO};
        bool mDepthTestEnabled {false};
        bool mDepthWriteEnabled {true};
        bool mCullFaceEnabled {false};
        bool mScissorTestEnabled {false};
        const ShaderProgram* mShaderProgram {nullptr};
        const Material* mMaterial {nullptr};
        const Vbo* mVbo {nullptr};
    };
}

#endif
