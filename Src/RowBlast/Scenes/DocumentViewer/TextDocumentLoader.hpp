#ifndef TextDocumentLoader_hpp
#define TextDocumentLoader_hpp

#include <string>

// Engine includes.
#include "Vector.hpp"

namespace Pht {
    class Scene;
    class ScrollPanel;
    class TextProperties;
}

namespace RowBlast {
    namespace TextDocumentLoader {
        void Load(Pht::Scene& scene,
                  Pht::ScrollPanel& panel,
                  const std::string& filename,
                  const Pht::TextProperties& textProperties,
                  const Pht::Vec3& upperLeft,
                  float lineSpacing,
                  int maxLineWidth);
    }
}

#endif
