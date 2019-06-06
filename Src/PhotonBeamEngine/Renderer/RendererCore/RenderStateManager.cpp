#include "RenderStateManager.hpp"

#include <assert.h>

#include "ShaderProgram.hpp"

using namespace Pht;

void RenderStateManager::Init() {
    UpdateGlBlend();
    UpdateGlBlendFunc();
    UpdateGlDepthTest();
    UpdateGlDepthWrite();
    UpdateGlCullFace();
    UpdateGlScissorTest();
}

void RenderStateManager::SetBlend(bool blendEnabled) {
    if (mBlendEnabled != blendEnabled) {
        mBlendEnabled = blendEnabled;
        UpdateGlBlend();
    }
}

void RenderStateManager::SetBlendFunc(GLenum sFactor, GLenum dFactor) {
    if (mBlendSFactor != sFactor || mBlendDFactor != dFactor) {
        mBlendSFactor = sFactor;
        mBlendDFactor = dFactor;
        UpdateGlBlendFunc();
    }
}

void RenderStateManager::SetDepthTest(bool depthTestEnabled) {
    if (mDepthTestEnabled != depthTestEnabled) {
        mDepthTestEnabled = depthTestEnabled;
        UpdateGlDepthTest();
    }
}

void RenderStateManager::SetDepthWrite(bool depthWriteEnabled) {
    if (mDepthWriteEnabled != depthWriteEnabled) {
        mDepthWriteEnabled = depthWriteEnabled;
        UpdateGlDepthWrite();
    }
}

void RenderStateManager::SetCullFace(bool cullFaceEnabled) {
    if (mCullFaceEnabled != cullFaceEnabled) {
        mCullFaceEnabled = cullFaceEnabled;
        UpdateGlCullFace();
    }
}

void RenderStateManager::SetScissorTest(bool scissorTestEnabled) {
    if (mScissorTestEnabled != scissorTestEnabled) {
        mScissorTestEnabled = scissorTestEnabled;
        UpdateGlScissorTest();
    }
}

void RenderStateManager::BindTexture(GLenum textureUnitIndex, GLenum target, GLuint texture) {
    auto* unit = GetTextureUnit(textureUnitIndex);
    if (unit == nullptr) {
        assert(false);
        return;
    }
    
    if (!unit->HasValue() ||
        unit->GetValue().mTarget != target || unit->GetValue().mTexture != texture) {
        
        *unit = TextureUnit {.mTarget = target, .mTexture = texture};
        
        glActiveTexture(textureUnitIndex);
        glBindTexture(target, texture);
        IF_USING_FRAME_STATS(ReportTextureBind());
    }
}

Optional<RenderStateManager::TextureUnit>* RenderStateManager::GetTextureUnit(GLenum unitIndex) {
    switch (unitIndex) {
        case GL_TEXTURE0:
            return &mTextureUnit0;
        case GL_TEXTURE1:
            return &mTextureUnit1;
        default:
            return nullptr;
    }
}

void RenderStateManager::UpdateGlBlend() {
    if (mBlendEnabled) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}

void RenderStateManager::UpdateGlBlendFunc() {
    glBlendFunc(mBlendSFactor, mBlendDFactor);
}

void RenderStateManager::UpdateGlDepthTest() {
    if (mDepthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void RenderStateManager::UpdateGlDepthWrite() {
    if (mDepthWriteEnabled) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }
}

void RenderStateManager::UpdateGlCullFace() {
    if (mCullFaceEnabled) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void RenderStateManager::UpdateGlScissorTest() {
    if (mScissorTestEnabled) {
        glEnable(GL_SCISSOR_TEST);
    } else {
        glDisable(GL_SCISSOR_TEST);
    }
}

void RenderStateManager::UseShader(ShaderProgram& shaderProgram) {
    InvalidateShader();
    assert(shaderProgram.IsEnabled());
    shaderProgram.Use();
    mShaderProgram = &shaderProgram;
    IF_USING_FRAME_STATS(++mFrameStats.mNumShaderUses);
}

void RenderStateManager::OnBeginRenderPass() {
    InvalidateShader();
}

void RenderStateManager::InvalidateShader() {
    mShaderProgram = nullptr;
    mMaterial = nullptr;
    mVbo = nullptr;    
    mTextureUnit0.Reset();
    mTextureUnit1.Reset();
}

#if USE_FRAME_STATS
void RenderStateManager::LogFrameStats(float frameSeconds) {
    std::cout << "=======================================" << std::endl
              << " FPS: " << 1.0f / frameSeconds << std::endl
              << " NumShaderUses: " << mFrameStats.mNumShaderUses << std::endl
              << " NumMaterialUses: " << mFrameStats.mNumMaterialUses << std::endl
              << " NumTextureBinds: " << mFrameStats.mNumTextureBinds << std::endl
              << " NumVboUses: " << mFrameStats.mNumVboUses << std::endl
              << " NumDrawCalls: " << mFrameStats.mNumDrawCalls << std::endl;
}
#endif
