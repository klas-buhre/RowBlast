#include "RenderableObject.hpp"

#include <assert.h>

#include "VertexBuffer.hpp"
#include "IMesh.hpp"
#include "VboCache.hpp"

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
    
    std::shared_ptr<Vbo> CreateVbo(RenderMode renderMode) {
        auto generateIndexBuffer =
            renderMode == RenderMode::Triangles ? GenerateIndexBuffer::Yes : GenerateIndexBuffer::No;
    
        return std::make_shared<Vbo>(generateIndexBuffer);
    }
}

RenderableObject::RenderableObject(const Material& material,
                                   const IMesh& mesh,
                                   const VertexFlags& flags) :
    mMaterial {material} {

    auto meshName = mesh.GetName();
    if (meshName.HasValue()) {
        mVbo = VboCache::Get(meshName.GetValue());
        if (mVbo == nullptr) {
            CreateVboAndUploadData(mesh, flags);
            VboCache::Add(meshName.GetValue(), mVbo);
        }
    } else {
        CreateVboAndUploadData(mesh, flags);
    }
}

RenderableObject::RenderableObject(const Material& material, RenderMode renderMode) :
    mRenderMode {renderMode},
    mMaterial {material},
    mVbo {CreateVbo(renderMode)} {}

RenderableObject::~RenderableObject() {}

void RenderableObject::CreateVboAndUploadData(const IMesh& mesh, const VertexFlags& flags) {
    mVbo = CreateVbo(mRenderMode);
    
    VertexBuffer vertexBuffer {mesh.GetVertices(flags)};
    UploadTriangles(vertexBuffer, BufferUsage::StaticDraw);
}

void RenderableObject::UploadTriangles(const VertexBuffer& vertexBuffer, BufferUsage bufferUsage) {
    assert(mRenderMode == RenderMode::Triangles);
    
    auto glBufferUsage = ToGlBufferUsage(bufferUsage);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVbo->mVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBuffer.GetVertexBufferSize() * sizeof(float),
                 vertexBuffer.GetVertexBuffer(),
                 glBufferUsage);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVbo->mIndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 vertexBuffer.GetIndexBufferSize() * sizeof(GLushort),
                 vertexBuffer.GetIndexBuffer(),
                 glBufferUsage);
    
    mVbo->mIndexCount = vertexBuffer.GetIndexBufferSize();
}

void RenderableObject::UploadPoints(const VertexBuffer& vertexBuffer, BufferUsage bufferUsage) {
    assert(mRenderMode == RenderMode::Points);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVbo->mVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBuffer.GetVertexBufferSize() * sizeof(float),
                 vertexBuffer.GetVertexBuffer(),
                 ToGlBufferUsage(bufferUsage));
    
    mVbo->mPointCount = vertexBuffer.GetNumVerticesWritten();
}

const Vbo& RenderableObject::GetVbo() const {
    return *mVbo;
}
