#ifndef RenderUtils_hpp
#define RenderUtils_hpp

// Game includes.
#include "FloatingCubes.hpp"

namespace Pht {
    class IRenderer;
    class Text;
}

namespace BlocksGame {
    class GradientRectangle;

    namespace RenderUtils {
        void RenderText(Pht::IRenderer& renderer, const Pht::Text& text);
        void RenderGradientRectangle(Pht::IRenderer& renderer, const GradientRectangle& rectangle);
    }
}

#endif