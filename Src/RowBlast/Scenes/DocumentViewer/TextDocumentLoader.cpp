#include "TextDocumentLoader.hpp"

// Engine includes.
#include "Scene.hpp"
#include "SceneObject.hpp"
#include "ScrollPanel.hpp"
#include "FileSystem.hpp"
#include "FileStorage.hpp"
#include "TextComponent.hpp"

using namespace RowBlast;

namespace {
    bool IsCharacterDelimiter(char character) {
        switch (character) {
            case ' ':
            case '\n':
            case '\t':
                return true;
            default:
                return false;
        }
    }
}

void TextDocumentLoader::Load(Pht::Scene& scene,
                              Pht::ScrollPanel& panel,
                              const std::string& filename,
                              const Pht::TextProperties& textProperties,
                              const Pht::Vec3& upperLeft,
                              float lineSpacing,
                              int maxLineWidth) {
    auto fullPath = Pht::FileSystem::GetResourceDirectory() + "/" + filename;
    std::string fileData;
    if (!Pht::FileStorage::LoadCleartextFile(fullPath, fileData)) {
        return;
    }
    
    auto textLinePosition = upperLeft;
    auto currentLineStartIndex = 0;
    auto currentWordStartIndex = 0;
    
    enum class ScanState {
        Word,
        Delimiter
    };
    
    auto scanState = ScanState::Delimiter;
    
    for (auto i = 0; i < fileData.size(); ++i) {
        auto character = fileData[i];
        auto currentLineWidth = i - currentLineStartIndex + 1;
        
        switch (scanState) {
            case ScanState::Delimiter:
                if (!IsCharacterDelimiter(character)) {
                    scanState = ScanState::Word;
                    currentWordStartIndex = i;
                }
                break;
            case ScanState::Word:
                if (IsCharacterDelimiter(character)) {
                    scanState = ScanState::Delimiter;
                }
                break;
        }
        
        if (character == '\n' || currentLineWidth >= maxLineWidth) {
            std::string textLine;
            
            if (character == '\n') {
                textLine = fileData.substr(currentLineStartIndex, currentLineWidth - 1);
                currentLineStartIndex = i + 1;
            } else if (scanState == ScanState::Word) {
                auto lineWidthExludingLastWord = currentWordStartIndex - currentLineStartIndex - 1;
                textLine = fileData.substr(currentLineStartIndex, lineWidthExludingLastWord);
                currentLineStartIndex = currentWordStartIndex;
            } else {
                textLine = fileData.substr(currentLineStartIndex, currentLineWidth);
                currentLineStartIndex = i + 1;
            }
            
            auto& textLineSceneObject = scene.CreateText(textLine, textProperties).GetSceneObject();
            textLineSceneObject.GetTransform().SetPosition(textLinePosition);
            panel.AddSceneObject(textLineSceneObject);
            
            textLinePosition.y -= lineSpacing;
        }
    }
}
