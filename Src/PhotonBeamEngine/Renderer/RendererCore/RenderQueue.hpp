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
        // Sort key bit layout.
        // Flags: bits 59-56:
        // Depth write flag: bit 59.
        static constexpr int depthWriteShift {59};
        static constexpr uint64_t depthWriteShiftedMask {uint64_t{1} << depthWriteShift};

        // Normal text flag: bit 58.
        static constexpr int normalTextShift {58};
        static constexpr uint64_t normalTextShiftedMask {uint64_t{1} << normalTextShift};

        // Top gradient text flag: bit 57.
        static constexpr int topGradientTextShift {57};
        static constexpr uint64_t topGradientTextShiftedMask {uint64_t{1} << topGradientTextShift};

        // Mid gradient text flag: bit 56.
        static constexpr int midGradientTextShift {56};
        static constexpr uint64_t midGradientTextShiftedMask {uint64_t{1} << midGradientTextShift};
        
        static constexpr uint64_t textFlagsShiftedMask {
            normalTextShiftedMask | topGradientTextShiftedMask | midGradientTextShiftedMask
        };

        // Shader id: bits 55-48:
        static constexpr int shaderIdShift {48};
        
        // Material id: bits 47-24:
        static constexpr int materialIdShift {24};
        
        // Vertex buffer id: bits 23-0:
        static constexpr int vertexBufferIdShift {0};
        
        struct Entry {
            uint64_t mSortKey;
            float mDistance;
            const SceneObject* mSceneObject;

            bool IsDepthWriting() const {
                return static_cast<bool>(mSortKey & depthWriteShiftedMask);
            }
            
            bool HasTextComponent() const {
                return static_cast<bool>(mSortKey & textFlagsShiftedMask);
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
        void ScanSubtree(const SceneObject& sceneObject, bool ancestorMatchedLayerMask);
        void AddSceneObject(const SceneObject& sceneObject);
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
