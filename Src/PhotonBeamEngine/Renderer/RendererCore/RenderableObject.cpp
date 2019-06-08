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
    
    std::shared_ptr<GpuVbo> CreateVbo(RenderMode renderMode) {
        auto generateIndexBuffer =
            renderMode == RenderMode::Triangles ? GenerateIndexBuffer::Yes : GenerateIndexBuffer::No;
    
        return std::make_shared<GpuVbo>(generateIndexBuffer);
    }
}

RenderableObject::RenderableObject(const Material& material,
                                   const IMesh& mesh,
                                   const VertexFlags& attributeFlags,
                                   VertexBufferLocation bufferLocation) :
    mMaterial {material} {

    auto meshName = mesh.GetName();
    if (meshName.HasValue()) {
        mVbo = VboCache::Get(meshName.GetValue());
        if (mVbo == nullptr) {
            mVbo = CreateVbo(mRenderMode);
            UploadMeshVertexData(mesh, attributeFlags, bufferLocation);
            VboCache::Add(meshName.GetValue(), mVbo);
        } else {
            if (bufferLocation == VertexBufferLocation::AtGpuAndCpu &&
                mVbo->GetCpuSideBuffer() == nullptr) {
                
                auto vertexBuffer = mesh.CreateVertexBuffer(attributeFlags);
                mVbo->SetCpuSideBuffer(std::move(vertexBuffer));
            }
        }
    } else {
        mVbo = CreateVbo(mRenderMode);
        UploadMeshVertexData(mesh, attributeFlags, bufferLocation);
    }
}

RenderableObject::RenderableObject(const Material& material,
                                   std::unique_ptr<VertexBuffer> cpuSideBuffer,
                                   RenderMode renderMode) :
    mRenderMode {renderMode},
    mMaterial {material},
    mVbo {CreateVbo(renderMode)} {

    mVbo->SetCpuSideBuffer(std::move(cpuSideBuffer));
}

RenderableObject::~RenderableObject() {}

void RenderableObject::UploadMeshVertexData(const IMesh& mesh,
                                            const VertexFlags& attributeFlags,
                                            VertexBufferLocation bufferLocation) {
    auto vertexBuffer = mesh.CreateVertexBuffer(attributeFlags);
    UploadTriangles(*vertexBuffer, BufferUsage::StaticDraw);
    
    if (bufferLocation == VertexBufferLocation::AtGpuAndCpu) {
        mVbo->SetCpuSideBuffer(std::move(vertexBuffer));
    }
}

void RenderableObject::UploadTriangles(BufferUsage bufferUsage) {
    auto* cpuSideBuffer = mVbo->GetCpuSideBuffer();
    assert(cpuSideBuffer);
    UploadTriangles(*cpuSideBuffer, bufferUsage);
}

void RenderableObject::UploadPoints(BufferUsage bufferUsage) {
    auto* cpuSideBuffer = mVbo->GetCpuSideBuffer();
    assert(cpuSideBuffer);
    UploadPoints(*cpuSideBuffer, bufferUsage);
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
