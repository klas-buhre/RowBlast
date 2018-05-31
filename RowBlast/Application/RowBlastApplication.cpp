/*
Backlog:
  -Engine:
    -Purchases.
    -Net/HTTP.
    -Analytics.

  -Gameplay:
   X-Speed up gameplay slightly? Maybe FallingPieceAnimation speed could depend on distance so that
     it always takes the same amount of time.
   X-Improve the AI move evaluation.
   X-Fix the piece pull down bug on level 31. See screenshot.
   X-When playing a level with clear objective the game could scroll up if there is no room to spawn
     the falling piece. If we are at the top of the level then it is game over, or maybe the top
     rows could be cleared so the the piece fits.
   X-Fix bug where the switchable pieces can change if double clocking on switch when the frame rate
     is really low like in the iPad simulator. 
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
   X-Maybe slightly less bevel in the blocks.
   X-Maybe the merging of two triangles into a cube could be done by letting the two triangles fade
     into the cube. Render the triangles and the cube at the same time, vary the opacity for the
     triangels from 1 to 0 and from 0 to 1 for the cube.
    -Maybe remove the menu window from the level completed view and remove the fade.
    -Could zoom in on map pin when clicking on a level in the map.
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
    -Dialog before level starts that states objectives and displays piece types.
        -Add background glow behind the blocks.
        -Bomb emission and rotation animations.


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
    -Camera shake.
        Cost: 3
        Done
    -Support for iPhone X screen.
        Cost: 10
        Done
    -Improving sliding text animation.
        Cost: 3
        Done
    -New GUI/Menus.
        Cost: 6
        Done
    -Animations/effects when clearing a level.
        Cost: 7
        Done
    -Dialog before level starts that states objectives and displays piece types.
        Cost: 2
    -Do the marked items in the backlog.
        Cost: 8
    -Moving the next level particle effect to the next level after clearing a level. Add an avatar?
        Cost: 2
    -Levels/Chapters.
        Cost: 10
    -Tutorial.
        Cost: 10
    -Store settings in file.
        Cost: 0.5
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

        Total: 172


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


Add these clouds before generating sky background texture from title scene:
        CloudPathVolume {
            .mPosition = {-7.5f, -47.0f, -100.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mCloudSize = {30.0f, 30.0f},
            .mNumClouds = 2,
        },
        CloudPathVolume {
            .mPosition = {-15.0f, -60.0f, -100.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mCloudSize = {30.0f, 30.0f},
            .mNumClouds = 2,
        },
        CloudPathVolume {
            .mPosition = {10.0f, -60.0f, -100.0f},
            .mSize = {0.0f, 0.0f, 0.0f},
            .mCloudSize = {25.0f, 25.0f},
            .mNumClouds = 2,
        },
 
 
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

big_button_048.obj:
size: 9.4x2.6
bevel width: 0.48
segments: 5

medium_button_0385.obj:
size: 9.0x2.1
bevel width: 0.385
segments: 5

small_button_0385.obj:
size: 3.6x2.1
bevel width: 0.385
segments: 5

close_button_032.obj:
size: 1.9x1.9
bevel width: 0.32
segments: 5


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
    mTitleController {engine, mCommonResources},
    mGameController {engine, mCommonResources, mUserData, mSettings},
    mMapController {
        engine,
        mCommonResources,
        mUserData,
        mSettings,
        mGameController.GetLevelResources(),
        mGameController.GetPieceResources()
    },
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
