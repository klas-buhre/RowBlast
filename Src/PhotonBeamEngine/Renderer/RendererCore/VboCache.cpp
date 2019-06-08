#include "VboCache.hpp"

#include <mutex>
#include <vector>

using namespace Pht;

namespace {
    static std::mutex mutex;
    static std::vector<std::pair<std::string, std::weak_ptr<GpuVbo>>> cache;
}

uint32_t GpuVbo::mIdCounter = 0;

GpuVbo::GpuVbo(GenerateIndexBuffer generateIndexBuffer) {
    glGenBuffers(1, &mVertexBufferId);
    
    if (generateIndexBuffer == GenerateIndexBuffer::Yes) {
        glGenBuffers(1, &mIndexBufferId);
    }
}

GpuVbo::~GpuVbo() {
    glDeleteBuffers(1, &mVertexBufferId);
    glDeleteBuffers(1, &mIndexBufferId);
}

std::shared_ptr<GpuVbo> VboCache::Get(const std::string& meshName) {
    std::lock_guard<std::mutex> guard {mutex};
    
    cache.erase(
        std::remove_if(
            std::begin(cache),
            std::end(cache),
            [] (const auto& entry) { return entry.second.lock() == std::shared_ptr<GpuVbo>(); }),
        std::end(cache));

    for (const auto& entry: cache) {
        if (auto vbo {entry.second.lock()}) {
            auto& key {entry.first};
            
            if (meshName == key) {
                return vbo;
            }
        }
    }
    
    return nullptr;
}

void VboCache::Add(const std::string& meshName, std::shared_ptr<GpuVbo> vbo) {
    std::lock_guard<std::mutex> guard {mutex};
    
    cache.emplace_back(meshName, vbo);
}
