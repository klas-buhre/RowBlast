#include "RenderBatcher.hpp"

#include <assert.h>

#include "SceneObject.hpp"
#include "RenderableObject.hpp"
#include "TextComponent.hpp"
#include "VboCache.hpp"

using namespace Pht;

namespace {
    const static Vec3 defaultRotation {0.0f, 0.0f, 0.0f};
    const static Vec3 defaultScale {1.0f, 1.0f, 1.0f};
    
    struct VertexBufferPtrs {
        float* mVertexWritePtr {nullptr};
        const float* mPastVertexBufferCapacity {nullptr};
        uint16_t* mIndexWritePtr {nullptr};
        const uint16_t* mPastIndexBufferCapacity {nullptr};
        uint16_t mNumIndices {0};
    };
    
    struct StaticBatchSetup {
        std::unique_ptr<VertexBuffer> mBatchVertexBuffer;
        const Material* mBatchMaterial {nullptr};
    };
    
    StaticBatchSetup SetUpStaticBatchIfPossible(const SceneObject& sourceSceneObject) {
        if (sourceSceneObject.GetRenderable() || sourceSceneObject.GetComponent<TextComponent>()) {
            return {nullptr, nullptr};
        }
        
        auto totalNumVertices = 0;
        auto totalNumIndices = 0;
        RenderableObject* previousRenderable {nullptr};
        
        for (auto* sceneObject: sourceSceneObject.GetChildren()) {
            if (sceneObject->GetComponent<TextComponent>() || !sceneObject->GetChildren().empty()) {
                return {nullptr, nullptr};
            }
            
            if (!sceneObject->IsVisible()) {
                continue;
            }
            
            auto* renderable = sceneObject->GetRenderable();
            if (renderable == nullptr) {
                continue;
            }
            
            auto& material = renderable->GetMaterial();
            if (material.GetOpacity() != 1.0f) {
                // Transparent objects need depth sorting based on camera position but the camera is
                // unknown during static batching.
                return {nullptr, nullptr};
            }
            
            auto& vbo = renderable->GetVbo();
            auto* vboCpuSideBuffer = vbo.GetCpuSideBuffer();
            if (vboCpuSideBuffer == nullptr) {
                return {nullptr, nullptr};
            }
            
            if (previousRenderable) {
                auto& previousMaterial = previousRenderable->GetMaterial();
                if (material.GetId() != previousMaterial.GetId() ||
                    material.GetShaderId() != previousMaterial.GetShaderId()) {
                    
                    return {nullptr, nullptr};
                }
                
                auto& previousVbo = previousRenderable->GetVbo();
                if (vboCpuSideBuffer->GetAttributeFlags() !=
                    previousVbo.GetCpuSideBuffer()->GetAttributeFlags()) {
                    
                    return {nullptr, nullptr};
                }
            }
            
            totalNumVertices += vboCpuSideBuffer->GetNumVertices();
            totalNumIndices += vboCpuSideBuffer->GetIndexBufferSize();
            previousRenderable = renderable;
        }

        if (totalNumVertices > 0 && totalNumIndices > 0 && previousRenderable) {
            auto& batchAttributeFlags =
                previousRenderable->GetVbo().GetCpuSideBuffer()->GetAttributeFlags();
            auto* batchMaterial = &previousRenderable->GetMaterial();
            return {
                std::make_unique<VertexBuffer>(totalNumVertices, totalNumIndices, batchAttributeFlags),
                batchMaterial
            };
        }
        
        return {nullptr, nullptr};
    }

    void WriteIndicesToBatch(VertexBufferPtrs& batchBufferPtrs,
                             const VertexBuffer& sceneObjectVertexBuffer) {
        auto sceneObjectNumIndices = sceneObjectVertexBuffer.GetNumIndices();
        if (sceneObjectNumIndices == 0) {
            return;
        }
    
        auto batchBufferNumIndices = batchBufferPtrs.mNumIndices;
        auto* indexWrite = batchBufferPtrs.mIndexWritePtr;
        auto* indexRead = sceneObjectVertexBuffer.GetIndexBuffer();
    
        assert(indexWrite + sceneObjectVertexBuffer.GetIndexBufferSize() <
               batchBufferPtrs.mPastIndexBufferCapacity);

        for (auto i = 0; i < sceneObjectNumIndices; ++i) {
            *indexWrite++ = batchBufferNumIndices + *indexRead++;
        }
    
        batchBufferPtrs.mIndexWritePtr = indexWrite;
        batchBufferPtrs.mNumIndices += sceneObjectNumIndices;
    }

    void TransformAndWriteVerticesToBatch(VertexBufferPtrs& batchBufferPtrs,
                                          const VertexBuffer& sceneObjectVertexBuffer,
                                          const Vec3& translation,
                                          const Vec3& scale) {
        auto attributeFlags = sceneObjectVertexBuffer.GetAttributeFlags();
        auto sceneObjectNumVertices = sceneObjectVertexBuffer.GetNumVertices();
        auto* vertexWrite = batchBufferPtrs.mVertexWritePtr;
        auto* vertexRead = sceneObjectVertexBuffer.GetVertexBuffer();
        
        assert(vertexWrite + sceneObjectVertexBuffer.GetVertexBufferSize() <
               batchBufferPtrs.mPastVertexBufferCapacity);
 
        for (auto i = 0; i < sceneObjectNumVertices; ++i) {
            // Vertex position is Vec3.
            *vertexWrite++ = *vertexRead++ * scale.x + translation.x;
            *vertexWrite++ = *vertexRead++ * scale.y + translation.y;
            *vertexWrite++ = *vertexRead++ * scale.z + translation.z;
            
            if (attributeFlags.mNormals) {
                // Vertex normal is Vec3.
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
            }
            
            if (attributeFlags.mTextureCoords) {
                // Vertex texture coord is Vec2.
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
            }
            
            if (attributeFlags.mColors) {
                // Vertex texture coord is Vec4.
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
            }
            
            if (attributeFlags.mPointSizes) {
                // Vertex texture coord is float.
                *vertexWrite++ = *vertexRead++;
            }
        }
        
        batchBufferPtrs.mVertexWritePtr = vertexWrite;
        
        WriteIndicesToBatch(batchBufferPtrs, sceneObjectVertexBuffer);
    }

    void TransformAndWriteVerticesToBatch(VertexBufferPtrs& batchBufferPtrs,
                                          const VertexBuffer& sceneObjectVertexBuffer,
                                          const Transform& sceneObjectTransform) {
/*
        auto attributeFlags = sceneObjectVertexBuffer.GetAttributeFlags();
        auto sceneObjectNumVertices = sceneObjectVertexBuffer.GetNumVertices();
        auto* vertexWrite = batchBufferPtrs.mVertexWritePtr;
        auto* vertexRead = sceneObjectVertexBuffer.GetVertexBuffer();
        
        assert(vertexWrite + sceneObjectVertexBuffer.GetVertexBufferSize() <
               batchBufferPtrs.mPastVertexBufferCapacity);
 
        for (auto i = 0; i < sceneObjectNumVertices; ++i) {
            // auto sceneObjectPosCamSpace = transposedViewMatrix * Vec4{sceneObjectPos, 1.0f};
            // Vertex position is Vec3.
            *vertexWrite++ = *vertexRead++ * scale.x + translation.x;
            *vertexWrite++ = *vertexRead++ * scale.y + translation.y;
            *vertexWrite++ = *vertexRead++ * scale.z + translation.z;
            
            if (attributeFlags.mNormals) {
                // Vertex normal is Vec3.
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
            }
            
            if (attributeFlags.mTextureCoords) {
                // Vertex texture coord is Vec2.
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
            }
            
            if (attributeFlags.mColors) {
                // Vertex texture coord is Vec4.
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
                *vertexWrite++ = *vertexRead++;
            }
            
            if (attributeFlags.mPointSizes) {
                // Vertex texture coord is float.
                *vertexWrite++ = *vertexRead++;
            }
        }
        
        batchBufferPtrs.mVertexWritePtr = vertexWrite;
        
        WriteIndicesToBatch(batchBufferPtrs, sceneObjectVertexBuffer);
*/
    }

    void WriteSceneObjectToBatch(VertexBufferPtrs& batchBufferPtrs,
                                 const VertexBuffer& sceneObjectVertexBuffer,
                                 const Transform& sceneObjectTransform) {
        if (sceneObjectTransform.GetRotation() == defaultRotation) {
            TransformAndWriteVerticesToBatch(batchBufferPtrs,
                                             sceneObjectVertexBuffer,
                                             sceneObjectTransform.GetPosition(),
                                             sceneObjectTransform.GetScale());
        } else {
            TransformAndWriteVerticesToBatch(batchBufferPtrs,
                                             sceneObjectVertexBuffer,
                                             sceneObjectTransform);
        }
    }

    std::unique_ptr<RenderableObject>
    CreateStaticBatchRenderable(VertexBuffer& batchVertexBuffer,
                                const SceneObject& sourceSceneObject,
                                const Material& batchMaterial,
                                const Optional<std::string>& batchVboName) {
        VertexBufferPtrs batchBufferPtrs {
            .mVertexWritePtr = batchVertexBuffer.GetVertexBuffer(),
            .mPastVertexBufferCapacity = batchVertexBuffer.GetPastVertexBufferCapacity(),
            .mIndexWritePtr = batchVertexBuffer.GetIndexBuffer(),
            .mPastIndexBufferCapacity = batchVertexBuffer.GetPastIndexBufferCapacity(),
        };
        
        for (auto* sceneObject: sourceSceneObject.GetChildren()) {
            if (!sceneObject->IsVisible()) {
                continue;
            }
            
            auto* renderable = sceneObject->GetRenderable();
            if (renderable == nullptr) {
                continue;
            }
            
            auto* sceneObjectVertexBuffer = renderable->GetVbo().GetCpuSideBuffer();
            auto& sceneObjectTransform = sceneObject->GetTransform();
            WriteSceneObjectToBatch(batchBufferPtrs,
                                    *sceneObjectVertexBuffer,
                                    sceneObjectTransform);
        }

        return std::make_unique<RenderableObject>(batchMaterial, batchVertexBuffer, batchVboName);
    }
}

std::unique_ptr<RenderableObject>
RenderBatcher::CreateStaticBatch(const SceneObject& sourceSceneObject,
                                 const Optional<std::string>& batchVboName) {
    auto staticBatchSetup = SetUpStaticBatchIfPossible(sourceSceneObject);
    if (staticBatchSetup.mBatchVertexBuffer == nullptr || staticBatchSetup.mBatchMaterial == nullptr) {
        return nullptr;
    }
    
    if (batchVboName.HasValue()) {
        auto vbo = VboCache::Get(batchVboName.GetValue());
        if (vbo == nullptr) {
            return CreateStaticBatchRenderable(*staticBatchSetup.mBatchVertexBuffer,
                                               sourceSceneObject,
                                               *staticBatchSetup.mBatchMaterial,
                                               batchVboName);
        } else {
            return std::make_unique<RenderableObject>(*staticBatchSetup.mBatchMaterial, vbo);
        }
    }
    
    return CreateStaticBatchRenderable(*staticBatchSetup.mBatchVertexBuffer,
                                       sourceSceneObject,
                                       *staticBatchSetup.mBatchMaterial,
                                       batchVboName);
}
