#include "RenderableObject.hpp"

#include <assert.h>

#include "VertexBuffer.hpp"

using namespace Pht;

RenderableObject::RenderableObject(const Material& material,
                                   GLuint vertexBufferId,
                                   GLuint indexBufferId,
                                   int indexCount) :
    mMaterial {material},
    mVertexBufferId {vertexBufferId},
    mIndexBufferId {indexBufferId},
    mIndexCount {indexCount} {}

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
