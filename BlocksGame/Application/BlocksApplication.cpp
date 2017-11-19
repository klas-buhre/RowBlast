/*
Backlog:
  -Engine:
    -The RenderableObject could be composed of a Material and shared_ptr to Vbo. Then there can be a
     VboCache in RenderCore. So many RenderableObjects could share a single Vbo. This means that it
     will be possible to have one RenderableObject per SceneObject (don't remove the shared_ptr to
     RenderableObject in SceneObject) so that the Material for a particular SceneObject can be
     modified without affecting the other SceneObjects.
    -Add a LightComponent with direction and intensity. The shaders need to be updated? Or is it
     enough to change the material uniforms?
    -Maybe introduce a Transform class for the SceneObject instead of the matrix?
  -Gameplay:
    -Fix all bugs.
    -Maybe a sensitivity setting. Tetris Blitz has lower sensitility on iPad but not on iPhone.
    -When playing a level with clear objective the game could scroll up if there is no room to spawn
     the falling piece. If we are at the top of the level then it is game over.
    -Undo to the previous move initial state or the the previous move last state before move? Should 
     not really matter since the user can select any piece from the selectables but maybe it is most 
     convenient to go back to the state before the move and not the initial state.
    -Mega bomb?
  -Rendering:
    -Fix preview pieces uggly patches.
    -Recreate the star particle. Possibly by buying a textures for particle systems in unity assets
     store. Look at: https://www.assetstore.unity3d.com/en/#!/content/26701
    -Could have a set of floating cube configurations for the game scene that always looks good.
    -Maybe the bombs could have constant particle effects as if it is about to explode. And they
     could be be of different shapes other than spheres.
    -Add heading to the green dialogs.
    -Try 1334×750 resolution for iPhone7.
    -Camera shake when clearing 5 rows.
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
    -The floating cubes could have materials with bought envmap textures without boundries. Or,
     The angle of the rotating cubes in the title scene and game scene could be configurable/
     deterministic so that the ugly edges in the env mapping is never seen.
    -MenuButtons can start a shrink animation when pressed down.
    -Try models with less bevel for the preview. The triangles with less bevel could be tried for
     field also.
  -Map:
    -Map can be a Pht::Scene.
    -Can have clouds in the map.
    -More 3D feel.
    -Load multiple maps.
  -Tutorial:
  -Purchases:
  -Back end:
  -Levels:
  -Sound:
    -Buy sounds and music.
  -Make ready for release:
    -Remove text from bottom right part of map texture.
    -Credit the icon creator: <div>Icons made by <a href="http://www.freepik.com" title="Freepik">Freepik</a> from <a href="http://www.flaticon.com" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a></div>
  
Ongoing tasks:
    -How to load/switch scene?
        -Remember to call RenderQueue::Allocate() when settting a scene.
        -Add SceneManager with methods SetScene and CreateRenderableObject. And maybe create methods
         for SceneObjects with CameraComponent, TextComponent or LightComponent.
 
Ideas:
    -The pause button could lead to a widget that has an undo button, a boosters button and a game
     menu button.
 

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
    mFadeEffect {engine, engine.GetRenderer(), 0.2f, 1.0f} {

    engine.GetInput().SetUseGestureRecognizers(false);
}

void BlocksApplication::Initialize() {
    mTitleController.Reset();
}

void BlocksApplication::Update() {
    std::cout << "******* FPS: " << 1.0f / mEngine.GetLastFrameSeconds() << std::endl;
    
    if (mState != State::GameScene) {
        mUserData.Update();
    }
    
    UpdateScene();
    HandleTransitions();
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
    if (mFadeEffect.UpdateAndRender(mEngine.GetLastFrameSeconds()) == Pht::FadeEffect::State::Transition) {
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
    mMapController.Reset();
}

void BlocksApplication::StartGame() {
    mState = State::GameScene;
    mMapController.Stop();
    mGameController.StartLevel(mLevelToStart);
}
