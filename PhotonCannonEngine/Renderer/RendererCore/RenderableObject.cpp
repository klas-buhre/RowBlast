#include "RenderableObject.hpp"

#include <assert.h>

#include "VertexBuffer.hpp"

using namespace Pht;

namespace {
    GLenum ToGlBufferUsage(BufferUsage bufferUsage) {
        switch (bufferUsage) {
            case BufferUsage::StaticDraw:
                return GL_STATIC_DRAW;
            case BufferUsage::DynamicDraw:
                return GL_DYNAMIC_DRAW;
        }
    }
}

RenderableObject::RenderableObject(const Material& material, const VertexBuffer& vertexBuffer) :
    mMaterial {material} {
    
    glGenBuffers(1, &mVertexBufferId);
    glGenBuffers(1, &mIndexBufferId);

    UploadTriangles(vertexBuffer, BufferUsage::StaticDraw);
}

RenderableObject::RenderableObject(const Material& material, RenderMode renderMode) :
    mRenderMode {renderMode},
    mMaterial {material} {
    
    glGenBuffers(1, &mVertexBufferId);
    
    if (renderMode == RenderMode::Triangles) {
        glGenBuffers(1, &mIndexBufferId);
    }
}

RenderableObject::~RenderableObject() {
    glDeleteBuffers(1, &mVertexBufferId);
    glDeleteBuffers(1, &mIndexBufferId);
}

void RenderableObject::UploadTriangles(const VertexBuffer& vertexBuffer, BufferUsage bufferUsage) {
    assert(mRenderMode == RenderMode::Triangles);
    
    auto glBufferUsage {ToGlBufferUsage(bufferUsage)};
    
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBuffer.GetVertexBufferSize() * sizeof(float),
                 vertexBuffer.GetVertexBuffer(),
                 glBufferUsage);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 vertexBuffer.GetIndexBufferSize() * sizeof(GLushort),
                 vertexBuffer.GetIndexBuffer(),
                 glBufferUsage);
    
    mIndexCount = vertexBuffer.GetIndexBufferSize();
}

void RenderableObject::UploadPoints(const VertexBuffer& vertexBuffer, BufferUsage bufferUsage) {
    assert(mRenderMode == RenderMode::Points);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBuffer.GetVertexBufferSize() * sizeof(float),
                 vertexBuffer.GetVertexBuffer(),
                 ToGlBufferUsage(bufferUsage));
    
    mPointCount = vertexBuffer.GetNumVerticesWritten();
}
