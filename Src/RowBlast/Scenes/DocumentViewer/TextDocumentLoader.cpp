#include "TextDocumentLoader.hpp"

// Engine includes.
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "FileSystem.hpp"
#include "FileStorage.hpp"
#include "TextComponent.hpp"

using namespace RowBlast;

void TextDocumentLoader::Load(Pht::Scene& scene,
                              Pht::SceneObject& container,
                              const std::string& filename,
                              const Pht::TextProperties& textProperties,
                              const Pht::Vec3& upperLeft,
                              float lineSpacing,
                              int maxLineWidth) {
    auto fullPath {Pht::FileSystem::GetResourceDirectory() + "/" + filename};
    std::string fileData;
    
    if (!Pht::FileStorage::LoadCleartextFile(fullPath, fileData)) {
        return;
    }
    
    auto textLinePosition {upperLeft};
    auto currentLineStartIndex {0};
    
    for (auto i {0}; i < fileData.size(); ++i) {
        auto character {fileData[i]};
        auto currentLineWidth {i - currentLineStartIndex + 1};
        
        if (character == '\n' || currentLineWidth >= maxLineWidth) {
            auto textLine {
                character == '\n' ? fileData.substr(currentLineStartIndex, currentLineWidth - 1) :
                fileData.substr(currentLineStartIndex, currentLineWidth)
            };
            
            auto& textLineSceneObject {scene.CreateText(textLine, textProperties).GetSceneObject()};
            textLineSceneObject.GetTransform().SetPosition(textLinePosition);
            container.AddChild(textLineSceneObject);
            
            textLinePosition.y -= lineSpacing;
            currentLineStartIndex = i + 1;
        }
    }
}
