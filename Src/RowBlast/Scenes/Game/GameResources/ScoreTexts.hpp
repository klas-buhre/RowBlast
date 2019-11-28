#ifndef ScoreTexts_hpp
#define ScoreTexts_hpp

#include <memory>
#include <string>
#include <vector>

// Engine includes.
#include "Vector.hpp"
#include "SceneObject.hpp"

namespace Pht {
    class Font;
}

namespace RowBlast {
    class GameScene;
    class CommonResources;
    
    class ScoreTexts {
    public:
        ScoreTexts(GameScene& scene, const CommonResources& commonResources);
        
        void Init();
        void Start(int numPoints, const Pht::Vec2& position);
        void Start(int numPoints, const Pht::Vec2& position, float delay);
        void Update(float dt);
        
    private:
        class ScoreText {
        public:
            ScoreText(GameScene& scene, const CommonResources& commonResources);
            
            void Init();
            void Start(int numPoints, const Pht::Vec2& position, float delay);
            void Update(float dt);
            bool IsInactive() const;
                        
        private:
            Pht::SceneObject& CreateText(const Pht::Font& font,
                                         const Pht::Vec2& position,
                                         const std::string& text);
            void UpdateInWaitingState(float dt);
            void UpdateInScalingUpState(float dt);
            void UpdateInSlidingUpState(float dt);
            void UpdateInSlidingUpFadingOutAndScalingDownState(float dt);
            
            enum class State {
                Waiting,
                ScalingUp,
                SlidingUp,
                SlidingUpFadingOutAndScalingDown,
                Inactive
            };
            
            GameScene& mScene;
            State mState {State::Inactive};
            float mElapsedTime {0.0f};
            float mDelay {0.0f};
            std::unique_ptr<Pht::SceneObject> mSceneObject;
            std::unique_ptr<Pht::SceneObject> mTextSceneObject;
        };

        std::vector<std::unique_ptr<ScoreText>> mScoreTexts;
    };
}

#endif
