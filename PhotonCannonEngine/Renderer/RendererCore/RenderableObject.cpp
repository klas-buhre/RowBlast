#include "RenderableObject.hpp"

#include <assert.h>

#include "VertexBuffer.hpp"

using namespace Pht;

RenderableObject::RenderableObject(const Material& material, const VertexBuffer& vertexBuffer) :
    mMaterial {material},
    mIndexCount {vertexBuffer.GetIndexBufferSize()} {
    
    glGenBuffers(1, &mVertexBufferId);
    glGenBuffers(1, &mIndexBufferId);

    UploadTriangles(vertexBuffer);
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

void RenderableObject::UploadTriangles(const VertexBuffer& vertexBuffer) {
    assert(mRenderMode == RenderMode::Triangles);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBuffer.GetVertexBufferSize() * sizeof(float),
                 vertexBuffer.GetVertexBuffer(),
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 vertexBuffer.GetIndexBufferSize() * sizeof(GLushort),
                 vertexBuffer.GetIndexBuffer(),
                 GL_STATIC_DRAW);
}

void RenderableObject::UploadPoints(const VertexBuffer& vertexBuffer) {
    assert(mRenderMode == RenderMode::Points);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBuffer.GetVertexBufferSize() * sizeof(float),
                 vertexBuffer.GetVertexBuffer(),
                 GL_DYNAMIC_DRAW);
    
    mPointCount = vertexBuffer.GetNumVerticesWritten();
}
