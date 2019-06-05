#ifndef RenderQueue_hpp
#define RenderQueue_hpp

#include <vector>

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Scene.hpp"
#include "RenderPass.hpp"

namespace Pht {
    class SceneObject;
    
    class RenderQueue {
    public:
        // Depth write: bit 63.
        static constexpr int depthWriteShift {63};
        static constexpr uint64_t depthWriteMask {uint64_t{1} << depthWriteShift};

        // Shader id: bits 62-56:
        
        // Material id: bits 55-28:
        
        // Vbo id: bits 27-0:
        
        struct Entry {
            uint64_t mSortKey;
            float mDistance;
            const SceneObject* mSceneObject;

            bool IsDepthWriting() const {
                return static_cast<bool>(depthWriteMask & mSortKey);
            }
        };

        void Init(const SceneObject& rootSceneObject);
        void Build(const Mat4& viewMatrix,
                   RenderOrder renderOrder,
                   DistanceFunction distanceFunction,
                   int layerMask);
        void BeginIteration();
        Entry& GetNextEntry();
        
        bool HasMoreEntries() const {
            return mIteratorIndex < mCurrentPartitionEndIndex;
        }

    private:
        void Sort();
        void AddSceneObject(const SceneObject& sceneObject, bool ancestorMatchedLayerMask);
        static uint64_t EncodeSortKey(bool isDepthWriting);
        void CalculateDistances(const Mat4& viewMatrix, DistanceFunction distanceFunction);
        
        template<typename Comparator>
        void SortFirstPartition();

        template<typename Comparator>
        void SortSecondPartition();

        const SceneObject* mRootSceneObject;
        RenderOrder mRenderOrder {RenderOrder::StateOptimized};
        int mLayerMask {0};
        std::vector<Entry> mQueue;
        int mFirstPartitionSize {0};
        int mSecondPartitionSize {0};
        int mMaxSize {0};
        int mIteratorIndex {0};
        int mCurrentPartitionEndIndex {0};
    };
}

#endif
