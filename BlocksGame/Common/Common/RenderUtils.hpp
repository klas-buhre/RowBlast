#ifndef RenderUtils_hpp
#define RenderUtils_hpp

// Game includes.
#include "FloatingCubes.hpp"

namespace Pht {
    class IRenderer;
    class ParticleEffect;
    class Text;
}

namespace BlocksGame {
    class GradientRectangle;

    namespace RenderUtils {
        void RenderParticleEffect(Pht::IRenderer& renderer, const Pht::ParticleEffect& effect);
        void RenderText(Pht::IRenderer& renderer, const Pht::Text& text);
        void RenderFloatingCubes(Pht::IRenderer& renderer, const std::vector<FloatingCube>& cubes);
        void RenderGradientRectangle(Pht::IRenderer& renderer, const GradientRectangle& rectangle);
    }
}

#endif
