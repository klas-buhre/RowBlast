#include "GuiViewManager.hpp"

#include <assert.h>

// Engine includes.
#include "GuiView.hpp"
#include "SceneObject.hpp"

using namespace BlocksGame;

void GuiViewManager::Init(Pht::SceneObject& parentObject) {
    for (auto* view: mViews) {
        view->SetIsActive(false);
        parentObject.AddChild(view->GetRoot());
    }
}

void GuiViewManager::AddView(Pht::GuiView& view) {
    mViews.push_back(&view);
}

void GuiViewManager::SetActiveView(int viewIndex) {
    for (auto* view: mViews) {
        view->SetIsActive(false);
    }
    
    assert(viewIndex < mViews.size());
    mViews[viewIndex]->SetIsActive(true);
}
