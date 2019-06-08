#include "RenderBatcher.hpp"

#include "SceneObject.hpp"
#include "RenderableObject.hpp"
#include "TextComponent.hpp"
#include "VboCache.hpp"

using namespace Pht;

namespace {
    using StaticBatchSetup = std::pair<std::unique_ptr<VertexBuffer>, const Material*>;
    
    StaticBatchSetup SetUpStaticBatchIfPossible(const SceneObject& fromSceneObject) {
        auto totalNumVertices = 0;
        auto totalNumIndices = 0;
        const VertexFlags* attributeFlags {nullptr};
        RenderableObject* previousRenderable {nullptr};
        
        for (auto* sceneObject: fromSceneObject.GetChildren()) {
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
            attributeFlags = &vboCpuSideBuffer->GetAttributeFlags();
            previousRenderable = renderable;
        }

        if (totalNumVertices > 0 && totalNumIndices > 0 && attributeFlags && previousRenderable) {
            return {
                std::make_unique<VertexBuffer>(totalNumVertices, totalNumIndices, *attributeFlags),
                &previousRenderable->GetMaterial()
            };
        }
        
        return {nullptr, nullptr};
    }
    
    std::unique_ptr<RenderableObject>
    CreateStaticBatchRenderable(const SceneObject& fromSceneObject,
                                VertexBuffer& batchVertexBuffer,
                                const Material& batchMaterial,
                                const Optional<std::string>& vboName) {
        for (auto* sceneObject: fromSceneObject.GetChildren()) {
            if (!sceneObject->IsVisible()) {
                continue;
            }
            
            auto* renderable = sceneObject->GetRenderable();
            if (renderable == nullptr) {
                continue;
            }
            
            // auto& material = renderable->GetMaterial();
            // auto& vbo = renderable->GetVbo();
            // auto* vboCpuSideBuffer = vbo.GetCpuSideBuffer();
            
            


            // Transform the vertices with the SceneObject transform matrix. Here in the static
            // batch we can create a fresh local transform matrix from the local Transform of the
            // SceneObject. That would not be a very good solution for aby dynamic batching though.
            // In dynamic batching, each SceneObject should have a local transform matrix (as well
            // as the old world transform matrix) wich would be updated when the scene is update,
            // so that the dynamic batcher don't have to create the local matrix every frame and
            // every object in the batch.
        }

        return std::make_unique<RenderableObject>(batchMaterial, batchVertexBuffer, vboName);
    }
}

std::unique_ptr<RenderableObject>
RenderBatcher::CreateStaticBatch(const SceneObject& fromSceneObject,
                                 const Optional<std::string>& vboName) {
    auto staticBatchSetup = SetUpStaticBatchIfPossible(fromSceneObject);
    if (staticBatchSetup.first == nullptr || staticBatchSetup.second == nullptr) {
        return nullptr;
    }
    
    auto& allocatedBatchVertexBuffer = *staticBatchSetup.first;
    auto& batchMaterial = *staticBatchSetup.second;
    
    if (vboName.HasValue()) {
        auto vbo = VboCache::Get(vboName.GetValue());
        if (vbo == nullptr) {
            return CreateStaticBatchRenderable(fromSceneObject,
                                               allocatedBatchVertexBuffer,
                                               batchMaterial,
                                               vboName);
        } else {
            return std::make_unique<RenderableObject>(batchMaterial, vbo);
        }
    }
    
    return CreateStaticBatchRenderable(fromSceneObject,
                                       allocatedBatchVertexBuffer,
                                       batchMaterial,
                                       vboName);
}
