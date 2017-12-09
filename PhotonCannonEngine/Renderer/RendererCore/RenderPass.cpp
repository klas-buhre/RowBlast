#include "RenderPass.hpp"

using namespace Pht;

RenderPass::RenderPass(int layerIndex) {
    AddLayer(layerIndex);
}

void RenderPass::SetScissorBox(const ScissorBox& scissorBox) {
    mScissorBox = scissorBox;
}

void RenderPass::AddLayer(int layerIndex) {
    mLayerMask |= (1 << layerIndex);
}
