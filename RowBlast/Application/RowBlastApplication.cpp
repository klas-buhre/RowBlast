/*
Backlog:
  -Engine:
    -Purchases.
    -Net/HTTP.
    -Analytics.
    -Metal.

  -Gameplay:
   X-An objective could be to bring down all asteroids.
   X-Fix the piece pull down bug on level 31. See screenshot.
   X-Fix bug where the switchable pieces can change if double clicking on switch when the frame rate
     is really low like in the iPad simulator.
    -Maybe the FieldAnalyzer should give more value to cleared level blocks than cleared piece
     blocks.
    -Would be good if the FieldAnalyzer could take cascading/gravity into account when calculating
     BurriedHolesAreaInVisibleRows, at least for SevenPiece and MirroredSevenPiece. Currently, the
     AI gives to low scores for moves that it wrongly thinks will cause a lot burried holes because
     it does not pull down the non-cleared part of for example the SevenPiece.
    -Maybe blocks in the visible part of the field could disappear on some levels. For each move a
     random block disappears.
    -Maybe init falling piece after SwitchPiece() in the same frame instead of the next.
    -Maybe a sensitivity setting. Tetris Blitz has lower sensitility on iPad but not on iPhone.
    -Undo to the previous move initial state or the the previous move last state before move? Should
     not really matter since the user can select any piece from the selectables but maybe it is most 
     convenient to go back to the state before the move and not the initial state.
  -Rendering:
    -Maybe less reflectivity in the gray and yellow field block materials.
    -Green and yellow field block materials could be a bit darker.
    -Maybe smooth the dark area in the field env map.
    -Could have the Roswell spaceship in the space world.
  -GUI:
    -Adjust the window sizes and header area sizes. Make it more like the CascadingDialogView.
    -Change the settings dialog window into a dark window.
    -Menu window that displays 3rd party lib credits.
  -Purchases:
  -Back end:
  -Levels:
  -Sound:
    -Buy sounds and music.
  -Make ready for release:
    -Make launch screens for all supported device resulutions. Should say Teleporter Studios.
    -Make icons and launch screens for all supported device resulutions.
    -Credit the icon creator: <div>Icons made by <a href="http://www.freepik.com" title="Freepik">Freepik</a> from <a href="http://www.flaticon.com" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a></div>
    -Credit FastNoise, MIT license: https://github.com/Auburns/FastNoise/

Ongoing tasks:
    -Space world.
      -Map scene.
        -Background texture.
        -New planets.
        -Star.
        -Asteroids.
      -Game scene:
        -Background textures.
        -Adjust field quad brightness.
        -Adjust background layer light.



Ideas:
    -The pause button could lead to a widget that has an undo button, a boosters button and a game
     menu button.
    -A booster could destroy all welds in the visible part of the field.

Ordered Backlog:
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
        Done
    -Do the marked non-bug items in the backlog.
        Cost: 8
        Done
    -Animated movement of an avatar to the next level after clearing a level.
        Cost: 2
        Done
    -Go from one world to another.
        Cost: 3
        Done
    -Worlds.
        Cost: 4
        Done
    -Levels in the first world.
        Cost: 5
        Done
    -Tutorial.
        Cost: 10
        Done
    -Redesign the game HUD.
        Cost: 5
        Done
    -Levels in the second world.
        Cost: 7
        Done
    -Finish the remaining title and map scene-related rendering tasks.
        Cost: 7
        Done
    -Space world.
        Cost: 10
    -Levels for the space world.
        Cost: 7
    -Finish the remaining game scene-related rendering tasks.
        Cost: 4
    -Net/HTTP.
        Cost: 10
    -Analytics.
        Cost: 5
    -Store settings in file.
        Cost: 0.5
    -Sounds/music.
        Cost: 5
    -Fix all bugs.
        Cost: 4
    -Purchases.
        Cost: 15
    -Login/sign up account.
        Cost: 10
    -Back end.
        Cost: ?

        Total cost: 217


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
        -Should level bombs be counted as full blocks that can be used to clear a row?
            -One idea was that rows containging level bombs could be cleared and the level bombs
             should not trigger since the only way of choosing not to use those level bombs would
             have been to fill the row. However, after testing it became clear that rows with level
             bombs were often cleared by mistake during for example cascading, or you would end up
             in a situation where you would have to clear them even though it will lead to failing
             the level. So it was decided that level bombs does not count as full blocks that can
             clear a row. It also solves the ambiguous case when landing on level bombs and at the
             same time clearing the row containg those level bombs and some extra bombs. Should all
             those level bombs be triggered or only the ones landed on?
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


Do this before making tutorial slides:
    -Remove floating blocks
    -Disable light animation
    -Disable camera shake
    -Disable bomb emission animation
    -Disable bomb rotation animation
Gimp rectangle select:
size: 481, 481


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
bevel width: 0.13
segments: 5
profile 0.5
Export settings:
    Forward: Y Forward
    Up: Z Up

cube_428:
bevel width: 0.13
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

medium_button_skewed_0385.obj:
size: 9.0x2.1
shear: 0.24
bevel width: 0.385
segments: 5

small_button_0385.obj:
size: 3.6x2.1
bevel width: 0.385
segments: 5


FreeType compilation:
Put the build_freetype.sh script in freetype root dir then execute it.
*/

#include "RowBlastApplication.hpp"

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
    mUniverse {},
    mTitleController {engine, mCommonResources, mUserData, mUniverse},
    mGameController {engine, mCommonResources, mUserData, mSettings},
    mMapController {
        engine,
        mCommonResources,
        mUserData,
        mSettings,
        mUniverse,
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
    
    auto currentLevelId {mUserData.GetProgressManager().GetCurrentLevel()};
    mMapController.GetScene().SetWorldId(mUniverse.CalcWorldId(currentLevelId));
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
        case State::TitleScene:
            UpdateTitleScene();
            break;
        case State::MapScene:
            UpdateMapScene();
            break;
        case State::GameScene:
            UpdateGameScene();
            break;
    }
}

void RowBlastApplication::UpdateTitleScene() {
    auto command {mTitleController.Update()};
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case TitleController::Command::None:
                break;
            case TitleController::Command::GoToMap:
                mFadeEffect.SetDuration(fadeDuration);
                BeginFadeToMap(MapController::State::Map);
                break;
        }
    }
}

void RowBlastApplication::UpdateMapScene() {
    auto command {mMapController.Update()};
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command.GetKind()) {
            case MapController::Command::None:
                break;
            case MapController::Command::StartGame:
                BeginFadeToGame(command.GetLevel());
                break;
            case MapController::Command::StartMap:
                BeginFadeToMap(MapController::State::Map);
                break;
        }
    }
}

void RowBlastApplication::UpdateGameScene() {
    auto command {mGameController.Update()};
    
    if (!mFadeEffect.IsFadingOut()) {
        switch (command) {
            case GameController::Command::None:
                break;
            case GameController::Command::GoToMap:
                mLevelToStart = mUserData.GetProgressManager().GetCurrentLevel() + 1;
                if (mUserData.GetProgressManager().ProgressedAtPreviousGameRound()) {
                    BeginFadeToMap(MapController::State::UfoAnimation);
                    mMapController.SetStartLevelDialogOnAnimationFinished(false);
                } else {
                    BeginFadeToMap(MapController::State::Map);
                }
                break;
            case GameController::Command::RestartLevel:
                BeginFadeToMap(MapController::State::LevelGoalDialog);
                break;
            case GameController::Command::GoToNextLevel:
                mLevelToStart = mUserData.GetProgressManager().GetCurrentLevel() + 1;
                if (mUserData.GetProgressManager().ProgressedAtPreviousGameRound()) {
                    BeginFadeToMap(MapController::State::UfoAnimation);
                    mMapController.SetStartLevelDialogOnAnimationFinished(true);
                } else {
                    BeginFadeToMap(MapController::State::LevelGoalDialog);
                }
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

void RowBlastApplication::BeginFadeToMap(MapController::State mapControllerInitialState) {
    mMapControllerInitialState = mapControllerInitialState;
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
    mEngine.GetInput().EnableInput();
    mMapController.Init();
    
    switch (mMapControllerInitialState) {
        case MapController::State::Map:
            break;
        case MapController::State::LevelGoalDialog:
            mMapController.GetScene().SetCameraAtLevel(mLevelToStart);
            mMapController.GoToLevelGoalDialogState(mLevelToStart);
            break;
        case MapController::State::UfoAnimation:
            mMapController.GoToUfoAnimationState(mLevelToStart);
            break;
        default:
            assert(!"Illegal map initial state");
            break;
    }
}

void RowBlastApplication::StartGame() {
    mState = State::GameScene;
    mGameController.StartLevel(mLevelToStart);
}
