#include "RenderableObject.hpp"

#include <assert.h>

#include "VertexBuffer.hpp"

using namespace Pht;

RenderableObject::RenderableObject(const Material& material, const VertexBuffer& vertexBuffer) :
    mMaterial {material},
    mIndexCount {vertexBuffer.GetIndexBufferSize()} {
    
    // Create the VBO for the vertices, normals and texture coords.
    glGenBuffers(1, &mVertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBuffer.GetVertexBufferSize() * sizeof(float),
                 vertexBuffer.GetVertexBuffer(),
                 GL_STATIC_DRAW);
    
    // Create the VBO for the indices.
    glGenBuffers(1, &mIndexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 vertexBuffer.GetIndexBufferSize() * sizeof(GLushort),
                 vertexBuffer.GetIndexBuffer(),
                 GL_STATIC_DRAW);
}

RenderableObject::RenderableObject(const Material& material) :
    mRenderMode {RenderMode::Points},
    mMaterial {material} {
    
    glGenBuffers(1, &mVertexBufferId);
}

RenderableObject::~RenderableObject() {
    glDeleteBuffers(1, &mVertexBufferId);
    glDeleteBuffers(1, &mIndexBufferId);
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
