#include "GuiViewManager.hpp"

#include <assert.h>

// Engine includes.
#include "GuiView.hpp"
#include "SceneObject.hpp"

using namespace RowBlast;

void GuiViewManager::Init(Pht::SceneObject& parentObject) {
    for (auto& viewEntry: mViews) {
        auto* view {viewEntry.second};
        view->SetIsActive(false);
        parentObject.AddChild(view->GetRoot());
    }
}

void GuiViewManager::AddView(int viewId, Pht::GuiView& view) {
    mViews[viewId] = &view;
}

void GuiViewManager::ActivateView(int viewIndex) {
    DeactivateAllViews();
    
    auto viewEntry {mViews.find(viewIndex)};
    
    if (viewEntry == std::end(mViews)) {
        assert(!"View not found");
    }
    
    viewEntry->second->SetIsActive(true);
}

void GuiViewManager::DeactivateAllViews() {
    for (auto& viewEntry: mViews) {
        auto* view {viewEntry.second};
        view->SetIsActive(false);
    }
}
