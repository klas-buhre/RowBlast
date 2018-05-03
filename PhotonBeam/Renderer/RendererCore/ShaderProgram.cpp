#include "ShaderProgram.hpp"

#include <iostream>

using namespace Pht;

namespace {
    GLuint CompileShader(const char* source, GLenum shaderType) {
        auto shaderHandle {glCreateShader(shaderType)};
        glShaderSource(shaderHandle, 1, &source, 0);
        glCompileShader(shaderHandle);
        
        GLint compileSuccess;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
        
        if (compileSuccess == GL_FALSE) {
            GLchar messages[256];
            glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
            std::cout << messages;
            exit(1);
        }
        
        return shaderHandle;
    }
    
    GLuint BuildProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
        auto vertexShader {CompileShader(vertexShaderSource, GL_VERTEX_SHADER)};
        auto fragmentShader {CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER)};
        
        auto programHandle {glCreateProgram()};
        glAttachShader(programHandle, vertexShader);
        glAttachShader(programHandle, fragmentShader);
        glLinkProgram(programHandle);
        
        GLint linkSuccess;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
        if (linkSuccess == GL_FALSE) {
            GLchar messages[256];
            glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
            std::cout << messages;
            exit(1);
        }
        
        return programHandle;
    }
}

ShaderProgram::ShaderProgram(VertexFlags vertexFlags) :
    mVertexFlags {vertexFlags} {}

void ShaderProgram::Build(const char* vertexShaderSource, const char* fragmentShaderSource) {
    mProgram = BuildProgram(vertexShaderSource, fragmentShaderSource);
    
    // Extract the handles to attributes.
    mAttributes.mPosition = glGetAttribLocation(mProgram, "Position");
    mAttributes.mNormal = glGetAttribLocation(mProgram, "Normal");
    mAttributes.mTextureCoord = glGetAttribLocation(mProgram, "TextureCoord");
    mAttributes.mColor = glGetAttribLocation(mProgram, "Color");
    mAttributes.mTextCoords = glGetAttribLocation(mProgram, "TextCoords");
    mAttributes.mPointSize = glGetAttribLocation(mProgram, "PointSize");
    
    // Extract the handles to uniforms.
    mUniforms.mProjection = glGetUniformLocation(mProgram, "Projection");
    mUniforms.mModelViewProjection = glGetUniformLocation(mProgram, "ModelViewProjection");
    mUniforms.mNormalMatrix = glGetUniformLocation(mProgram, "NormalMatrix");
    mUniforms.mLightPosition = glGetUniformLocation(mProgram, "LightPosition");
    mUniforms.mAmbientMaterial = glGetUniformLocation(mProgram, "AmbientMaterial");
    mUniforms.mDiffuseMaterial = glGetUniformLocation(mProgram, "DiffuseMaterial");
    mUniforms.mSpecularMaterial = glGetUniformLocation(mProgram, "SpecularMaterial");
    mUniforms.mEmissiveMaterial = glGetUniformLocation(mProgram, "EmissiveMaterial");
    mUniforms.mShininess = glGetUniformLocation(mProgram, "Shininess");
    mUniforms.mReflectivity = glGetUniformLocation(mProgram, "Reflectivity");
    mUniforms.mOpacity = glGetUniformLocation(mProgram, "Opacity");
    mUniforms.mTextColor = glGetUniformLocation(mProgram, "TextColor");
    mUniforms.mTextColorSubtraction = glGetUniformLocation(mProgram, "TextColorSubtraction");
    mUniforms.mSampler = glGetUniformLocation(mProgram, "Sampler");
    mUniforms.mEmissionSampler = glGetUniformLocation(mProgram, "EmissionSampler");
    mUniforms.mModel = glGetUniformLocation(mProgram, "Model");
    mUniforms.mModel3x3 = glGetUniformLocation(mProgram, "Model3x3");
    mUniforms.mCameraPosition = glGetUniformLocation(mProgram, "CameraPosition");
}

void ShaderProgram::SetProjection(const Mat4& projectionMatrix) {
    glUseProgram(mProgram);
    glUniformMatrix4fv(mUniforms.mProjection, 1, 0, projectionMatrix.Pointer());
}

void ShaderProgram::SetLightPosition(const Vec3& lightPosition) {
    glUseProgram(mProgram);
    glUniform3fv(mUniforms.mLightPosition, 1, lightPosition.Pointer());
}

void ShaderProgram::Use() const {
    glUseProgram(mProgram);
}
