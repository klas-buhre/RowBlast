/*
Backlog:
  -Engine:
    -Physics.
    -Purchases.
    -Net/HTTP.
    -Analytics.
    
  -Gameplay:
    -Maybe all neighbouring blocks with the same color should be connected with welds?
    -Maybe blocks in the visible part of the field could disappear on some levels. For each move a
     random block disappears.
    -Fix all bugs.
    -Maybe a sensitivity setting. Tetris Blitz has lower sensitility on iPad but not on iPhone.
    -When playing a level with clear objective the game could scroll up if there is no room to spawn
     the falling piece. If we are at the top of the level then it is game over.
    -Undo to the previous move initial state or the the previous move last state before move? Should 
     not really matter since the user can select any piece from the selectables but maybe it is most 
     convenient to go back to the state before the move and not the initial state.
    -Mega bomb?
  -Rendering:
    -Recreate the star particle. Possibly by buying a textures for particle systems in unity assets
     store. Look at: https://www.assetstore.unity3d.com/en/#!/content/26701
    -Maybe the bombs could have constant particle effects as if it is about to explode. And they
     could be be of different shapes other than spheres.
    -Add heading to the green dialogs.
    -Try 1334×750 resolution for iPhone7.
    -Camera shake when clearing 5 rows or using a bomb.
    -Try adding welds to the level blocks.
    -One or two stars in level cleared dialog could be placed more in the middle.
    -There could be a particle effect behind the stars in the level cleared dialog. Stars could also
     rotate in around the y axis for about a second and then only rotate around the z axis. During
     this short animation the stars could be scaled up as well.
    -The sliding text could slide in from the left and right; one row from the right and the other
     row from the left like in Stellar. The font could be similar to ethnocentric and have a 
     gradient effect like this in the text shader:
     gl_FragColor = vec4(TextureCoordOut.y, 1.0, 1.0, texture2D(Sampler, TextureCoordOut).a) * TextColor;
     The title text can also have that gradient.
    -Could have a rotating particle/flare in the sliding text animation.
    -The switching of pieces in the selectable pieces HUD could be animated.
    -Could have particles in the FlyingBlocksAnimation in the points from which the blocks gets
     tossed.
    -The blocks in FlyingBlocksAnimation should bounce off each other instead of passing right 
     through.
    -MenuButtons can start a shrink animation when pressed down.
    -Try models with less bevel for the preview. The triangles with less bevel could be tried for
     field also.
  -Tutorial:
  -Purchases:
  -Back end:
  -Levels:
  -Sound:
    -Buy sounds and music.
  -Make ready for release:
    -Credit the icon creator: <div>Icons made by <a href="http://www.freepik.com" title="Freepik">Freepik</a> from <a href="http://www.flaticon.com" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a></div>
  
Ongoing tasks:
    -GameScene should be a Pht::Scene.

Ideas:
    -The pause button could lead to a widget that has an undo button, a boosters button and a game
     menu button.
    -A booster could destroy all welds in the visible part of the field.

Time Estimation in days:
    -Convert GameScene into a Pht::Scene.
        Cost: 10
        Done
    -Let each GuiView be a Pht::Scene that can be merged into the current scene.
        Cost: 5
    -Finalize gameplay.
        Cost: 10
    -Fix all gameplay bugs.
        Cost: 7
    -Better particle effects for bombs and row bombs.
        Cost: 10
    -Bombs should have bomb meshes.
        Cost: 5
    -Camera shake.
        Cost: 3
    -Switch piece HUD animation.
        Cost: 3
    -Use new GUI/Menu textures.
        Cost: 3
    -Animations/effects when clearing a level.
        Cost: 7
    -Improving sliding text animation.
        Cost: 3
    -Tutorial.
        Cost: 10
    -Levels.
        Cost: 10
    -Purchases.
        Cost: 15
    -Sounds/music.
        Cost: 5
    -Net/HTTP.
        Cost: 10
    -Analytics.
        Cost: 5
    -Login/sign up account.
        Cost: 10
    -Back end.
    -Physics.
        Cost: 15

        Total: 146

Create rounded cube in Blender:
Scale cube so that size is 1.0. Select the wrench to the right.
Then, Add Modifier > Bevel > turn up width > turn up segments.

triangle_428:
bevel width: 0.085
segments: 5
profile 0.5
Rotated 90 degrees and forward: z direction in export settings.

cube_428:
bevel width: 0.135
weld size: 0.19
segments: 5
profile 0.5

FreeType compilation:
Put the build_freetype.sh script in freetype root dir then execute it.
*/

#include "BlocksApplication.hpp"

#include <iostream>

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"

using namespace BlocksGame;

std::unique_ptr<Pht::IApplication> CreateApplication(Pht::IEngine& engine) {
    return std::make_unique<BlocksApplication>(engine);
}

BlocksApplication::BlocksApplication(Pht::IEngine& engine) :
    mEngine {engine},
    mCommonResources {engine},
    mSettings {},
    mUserData {},
    mCommonViewControllers {engine, mCommonResources, mUserData, mSettings},
    mTitleController {engine, mCommonResources},
    mMapController {engine, mCommonResources, mCommonViewControllers, mUserData},
    mGameController {engine, mCommonResources, mCommonViewControllers, mUserData, mSettings},
    mFadeEffect {engine.GetSceneManager(), engine.GetRenderer(), 0.22f, 1.0f} {

    engine.GetInput().SetUseGestureRecognizers(false);
}

void BlocksApplication::OnUpdate() {
    std::cout << "******* FPS: " << 1.0f / mEngine.GetLastFrameSeconds() << std::endl;
    
    if (mState != State::GameScene) {
        mUserData.Update();
    }
    
    UpdateScene();
    HandleTransitions();
}

void BlocksApplication::OnRender() {
    switch (mState) {
        case State::TitleScene:
            mTitleController.RenderScene();
            break;
        case State::MapScene:
            mMapController.RenderScene();
            break;
        case State::GameScene:
            mGameController.RenderScene();
            break;
    }
    
    if (mFadeEffect.GetState() != Pht::FadeEffect::State::Idle) {
        mFadeEffect.Render();
    }
}

void BlocksApplication::UpdateScene() {
    switch (mState) {
        case State::TitleScene: {
            auto command {mTitleController.Update()};
            if (!mFadeEffect.IsFadingOut() && command == TitleController::Command::GoToMap) {
                BeginFadeToMap();
            }
            break;
        }
        case State::MapScene: {
            auto command {mMapController.Update()};
            if (!mFadeEffect.IsFadingOut() && command.GetKind() == MapController::Command::StartGame) {
                BeginFadeToGame(command.GetLevel());
            }
            break;
        }
        case State::GameScene:
            UpdateGameScene();
            break;
    }
}

void BlocksApplication::UpdateGameScene() {
    auto command {mGameController.Update()};
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case GameController::Command::None:
                break;
            case GameController::Command::GoToMap:
                BeginFadeToMap();
                break;
            case GameController::Command::RestartGame:
                BeginFadeToGame(mLevelToStart);
                break;
            case GameController::Command::GoToNextLevel:
                BeginFadeToGame(mUserData.GetProgressManager().GetCurrentLevel() + 1);
                break;
        }
    }
}

void BlocksApplication::HandleTransitions() {
    if (mFadeEffect.Update(mEngine.GetLastFrameSeconds()) == Pht::FadeEffect::State::Transition) {
        switch (mNextState) {
            case State::MapScene:
                StartMap();
                break;
            case State::GameScene:
                StartGame();
                break;
            default:
                break;
        }
    }
}

void BlocksApplication::BeginFadeToMap() {
    mFadeEffect.Start();
    mNextState = State::MapScene;
}

void BlocksApplication::BeginFadeToGame(int level) {
    mLevelToStart = level;
    mFadeEffect.Start();
    mNextState = State::GameScene;
}

void BlocksApplication::StartMap() {
    mState = State::MapScene;
    mMapController.Init();
}

void BlocksApplication::StartGame() {
    mState = State::GameScene;
    mGameController.StartLevel(mLevelToStart);
}
