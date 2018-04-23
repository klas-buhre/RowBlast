/*
Backlog:
  -Engine:
    -Purchases.
    -Net/HTTP.
    -Analytics.
    
  -Gameplay:
   X-Fix the piece pull down bug on level 31. See screenshot.
   X-Improve the AI move evaluation.
   X-When playing a level with clear objective the game could scroll up if there is no room to spawn
     the falling piece. If we are at the top of the level then it is game over, or maybe the top
     rows could be cleared so the the piece fits.
   X-Should level bombs be triggered when a piece lands partially on them? Try it out.
    -Speed up gameplay by updating GameLogic already when falling blocks are close to enter the
     bouncing state? If cascading, wait until all bocks have entered bouncing state before claring
     new rows.
    -Some levels could have som pink striped level blocks with welds that are affected by graivty.
    -Maybe blocks in the visible part of the field could disappear on some levels. For each move a
     random block disappears.
    -Scroll sideways in some levels? And/or already have colored piece blocks in the level before
     any pieces are landed?
    -Maybe init falling piece after SwitchPiece() in the same frame instead of the next.
    -Maybe a sensitivity setting. Tetris Blitz has lower sensitility on iPad but not on iPhone.
    -Undo to the previous move initial state or the the previous move last state before move? Should
     not really matter since the user can select any piece from the selectables but maybe it is most 
     convenient to go back to the state before the move and not the initial state.
  -Rendering:
    -Could try reducing scroll speed in map scene using a spring.
    -The title text could appear with the sliding text animation and with a quad behind it to give
     contrast.
    -Could zoom in on map pin when clicking on a level in the map.
    -Could have the rounded cylinder i some places in the HUDs.
    -GUI: the buttons in the views could be yellow with black text and triangular edges like in
     Mario Kart 8. There could also be a reflection animation in the button.
    -Maybe the merging of two triangles into a cube could be done by letting the two triangles fade
     into the cube. Render the triangles and the cube at the same time, vary the opacity for the
     triangels from 1 to 0 and from 0 to 1 for the cube.
    -Camera shake when clearing 3 or more rows or using a bomb.
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
    -Could have particles in the FlyingBlocksAnimation in the points from which the blocks gets
     tossed.
    -MenuButtons can start a shrink animation when pressed down.
  -Tutorial:
    -Make the tutorial as short as possible. Don't give all the rules of the game at once. Instead,
     let the user figure it out by playing. For example, introduce changing the visible moves in the
     second or third level. Let the first level introduce the rule of clearing lines and selecting
     moves and switching pieces.
  -Purchases:
  -Back end:
  -Levels:
  -Sound:
    -Buy sounds and music.
  -Make ready for release:
    -Make icons and launch screens for all supported device resulutions.
    -Credit the icon creator: <div>Icons made by <a href="http://www.freepik.com" title="Freepik">Freepik</a> from <a href="http://www.flaticon.com" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a></div>
    -Credit FastNoise, MIT license: https://github.com/Auburns/FastNoise/

Ongoing tasks:


Ideas:
    -The pause button could lead to a widget that has an undo button, a boosters button and a game
     menu button.
    -A booster could destroy all welds in the visible part of the field.

Time Estimation in days:
    -Convert GameScene into a Pht::Scene.
        Cost: 10
        Done
    -Let each GuiView be a subscene that can be merged into the current scene.
        Cost: 5
        Done
    -Finalize gameplay.
        Cost: 10
        Done
    -Fix all bugs.
        Cost: 7
        Done
    -Preview piece animation.
        Cost: 5
        Done
    -Block bounce.
        Cost: 5
        Done
    -Better particle effects for bombs and row bombs.
        Cost: 10
        Done
    -Bombs should have bomb meshes.
        Cost: 5
        Done
    -Try bombs part of level.
        Cost: 10
        Done
    -Do the marked items in the backlog.
        Cost: 8
    -Camera shake.
        Cost: 3
        Done
    -Use new GUI/Menu textures.
        Cost: 3
    -Support for iPhone X screen.
        Cost: 10
    -Animations/effects when clearing a level.
        Cost: 7
    -Moving the next level particle effect to the next level after clearing a level.
        Cost: 2
    -Improving sliding text animation.
        Cost: 3
    -Tutorial.
        Cost: 10
    -Levels/Chapters.
        Cost: 10
    -Sounds/music.
        Cost: 5
    -Purchases.
        Cost: 15
    -Net/HTTP.
        Cost: 10
    -Analytics.
        Cost: 5
    -Login/sign up account.
        Cost: 10
    -Back end.
        Cost: ?

        Total: 167


Comomon piece type sets:
    Easy:               RGBY:
        "LongI",           Y
        "I",             G
        "L",               Y
        "B",              B
        "D",            R
        "Seven",          B
        "MirroredSeven"  G
    Triangles:          RGBY:
        "LongI",           Y
        "I",             G
        "L",               Y
        "B",              B
        "D",            R
        "Triangle",     R
        "BigTriangle",    B
        "Bomb",
        "RowBomb"
    Only triangles:     RGBY:
        "SmallTriangle",R
        "Triangle",     R
        "BigTriangle",    B
        "Diamond",       G
        "Pyramid",         Y
        "Bomb",
        "RowBomb"
    Medium:             RGBY:
        "LongI",           Y
        "I",             G
        "L",               Y
        "B",              B
        "D",            R
        "Seven",          B
        "MirroredSeven", G
        "F",            R
        "MirroredF"       B
    Medium, a variant:  RGBY:
        "LongI",           Y
        "I",             G
        "L",               Y
        "B",              B
        "D",            R
        "Seven",          B
        "MirroredSeven", G
        "Z",               Y
        "MirroredZ"      G
    Hard:               RGBY:
        "LongI",           Y
        "I",             G
        "L",               Y
        "B",              B
        "D",            R
        "F",            R
        "MirroredF",      B
        "BigL",          G
        "Z",               Y
        "MirroredZ"      G
    Hard, variant:      RGBY:
        "LongI",           Y
        "ShortI",         B
        "L",               Y
        "B",              B
        "D",            R
        "F",            R
        "MirroredF",      B
        "BigL",          G
        "Z",               Y
        "MirroredZ"      G
    Hard, variant:      RGBY:
        "LongI",           Y
        "I",             G
        "B",              B
        "D",            R
        "F",            R
        "MirroredF",      B
        "BigL",          G
        "Z",               Y
        "MirroredZ"      G
        "T",              B
    Really hard:        RGBY:
        "LongI",           Y
        "I",             G
        "L",               Y
        "B",              B
        "D",            R
        "F",            R
        "MirroredF",      B
        "BigL",          G
        "Z",               Y
        "MirroredZ"      G
        "Plus"            B
    All pieces:         RGBY:
        "LongI",           Y
        "I",             G
        "L",               Y
        "B",              B
        "D",            R
        "Seven",          B
        "MirroredSeven"  G
        "Triangle",     R
        "BigTriangle",    B
        "SmallTriangle",R
        "Diamond",       G
        "Pyramid",         Y
        "F",            R
        "MirroredF",      B
        "BigL",          G
        "Z",               Y
        "MirroredZ"      G
        "T",              B
        "Plus"            B

        Color:    NumbBlocks:
        Red             12.5
        Green           18.0
        Blue            27.5
        Yellow          15.0
 
 
Decision notes on level bombs:
        -If clearing a full row containing some level bombs inside, should level bombs be
         triggered? Or, should they only be triggered when hit by pieces or explosions/lasers?
            -They should not trigger since they only way of choosing not to use those level bombs is
             to fill the row.
        -If clearing a full row containing a level bomb inside and the piece lands on it, should the
         level bomb be triggered?
            -Yes, it should be triggered since it is landed on but level bombs next to the hit level
             bomb should not trigger since they are cleared because of the full row.
        -If level bombs get pulled down and land should they trigger?
            -They should not triggering since this could open up for more strategic play by letting
             the player move level bombs to different rows.
        -If pulled down piece blocks (not gray level blocks) land on level bombs should they trigger?
            -In one way it is logical if they trigger since level bombs trigger when pieces land on
             them. On the other hand, it seems to be hard for the player to prevent pulled down
             blocks from accidentaly hitting bombs at lower rows. Probably tricky to implement but
             it might go something like this:
             during pulling down of pieces, Field detects that a level bomb will eventually be
             land on by piece blocks. The Field then adds that event to GameLogic which will
             detonate the level bomb once it is Updated again (after the CollapsingFieldAnimation is
             done with dragging down the piece blocks position).
 
 
Create rounded cube in Blender:
Scale cube so that size is 1.0. Select the wrench to the right.
Then, Add Modifier > Bevel > turn up width > turn up segments.

triangle_320:
bevel width: 0.135
segments: 5
profile 0.5
Export settings:
    Forward: Y Forward
    Up: Z Up

cube_428:
bevel width: 0.135
weld size: 0.19
segments: 5
profile 0.5

FreeType compilation:
Put the build_freetype.sh script in freetype root dir then execute it.
*/

#include "RowBlastApplication.hpp"

#include <iostream>

// Engine includes.
#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace RowBlast;

namespace {
    constexpr auto fadeDuration {0.22f};
    constexpr auto titleFadeInDuration {1.0f};
}

std::unique_ptr<Pht::IApplication> CreateApplication(Pht::IEngine& engine) {
    return std::make_unique<RowBlastApplication>(engine);
}

RowBlastApplication::RowBlastApplication(Pht::IEngine& engine) :
    mEngine {engine},
    mCommonResources {engine},
    mSettings {},
    mUserData {},
    mCommonViewControllers {engine, mCommonResources, mUserData, mSettings},
    mTitleController {engine, mCommonResources},
    mMapController {engine, mCommonResources, mCommonViewControllers, mUserData},
    mGameController {engine, mCommonResources, mCommonViewControllers, mUserData, mSettings},
    mFadeEffect {engine.GetSceneManager(), engine.GetRenderer(), titleFadeInDuration, 1.0f, 0.0f} {

    engine.GetInput().SetUseGestureRecognizers(false);
    
    auto& renderer {engine.GetRenderer()};
    renderer.DisableShader(Pht::ShaderType::PixelLighting);
    renderer.DisableShader(Pht::ShaderType::PointParticle);
    
    mFadeEffect.GetSceneObject().SetLayer(GlobalLayer::sceneSwitchFadeEffect);
    InsertFadeEffectInActiveScene();
    mFadeEffect.StartInMidFade();
}

void RowBlastApplication::OnUpdate() {
    if (mState != State::GameScene) {
        mUserData.Update();
    }
    
    UpdateScene();
    HandleTransitions();
}

void RowBlastApplication::UpdateScene() {
    switch (mState) {
        case State::TitleScene: {
            auto command {mTitleController.Update()};
            if (!mFadeEffect.IsFadingOut() && command == TitleController::Command::GoToMap) {
                mFadeEffect.SetDuration(fadeDuration);
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

void RowBlastApplication::UpdateGameScene() {
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

void RowBlastApplication::HandleTransitions() {
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
        
        InsertFadeEffectInActiveScene();
    }
}

void RowBlastApplication::InsertFadeEffectInActiveScene() {
    auto* scene {mEngine.GetSceneManager().GetActiveScene()};
    scene->GetRoot().AddChild(mFadeEffect.GetSceneObject());
}

void RowBlastApplication::BeginFadeToMap() {
    mFadeEffect.Start();
    mNextState = State::MapScene;
    mEngine.GetInput().DisableInput();
}

void RowBlastApplication::BeginFadeToGame(int level) {
    mLevelToStart = level;
    mFadeEffect.Start();
    mNextState = State::GameScene;
    mEngine.GetInput().DisableInput();
}

void RowBlastApplication::StartMap() {
    mState = State::MapScene;
    mMapController.Init();
    mEngine.GetInput().EnableInput();
}

void RowBlastApplication::StartGame() {
    mState = State::GameScene;
    mGameController.StartLevel(mLevelToStart);
}
