/*
Backlog:
  -Engine:
    -Physics.
    -Purchases.
    -Net/HTTP.
    -Analytics.
    
  -Gameplay:
    -Maybe init falling piece after SwitchPiece() in the same frame instead of the next.
    -Maybe blocks in the visible part of the field could disappear on some levels. For each move a
     random block disappears.
    -Maybe a sensitivity setting. Tetris Blitz has lower sensitility on iPad but not on iPhone.
    -When playing a level with clear objective the game could scroll up if there is no room to spawn
     the falling piece. If we are at the top of the level then it is game over.
    -Undo to the previous move initial state or the the previous move last state before move? Should 
     not really matter since the user can select any piece from the selectables but maybe it is most 
     convenient to go back to the state before the move and not the initial state.
    -Mega bomb?
  -Rendering:
    -Maybe increase field quad opacity to 0.9?
    -Maybe make the field quad as bright at the bottom as the top?
    -Try ghost pieces with very faint fill?
    -The cloud background in the game scene can be a bit zoomed out.
    -Maybe the blocks could be compressed a bit when landing after falling to avoid discontinuous
     derivative.
    -Could have the rounded cylinder i some places in the HUDs.
    -GUI: the buttons in the views could be yellow with black text and triangular edges like in
     Mario Kart 8. There could also be a reflection animation in the button.
    -Recreate the star particle. Possibly by buying a textures for particle systems in unity assets
     store. Look at: https://www.assetstore.unity3d.com/en/#!/content/26701
    -The PieceDropParticleEffect should be less noticeable.
    -Maybe add a time duration for each rotation in the FallingPieceAnimation.
    -Maybe the bombs could have constant particle effects as if it is about to explode. And they
     could be be of different shapes other than spheres.
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
  -Tutorial:
  -Purchases:
  -Back end:
  -Levels:
  -Sound:
    -Buy sounds and music.
  -Make ready for release:
    -Make icons and launch screens for all supported device resulutions.
    -Credit the icon creator: <div>Icons made by <a href="http://www.freepik.com" title="Freepik">Freepik</a> from <a href="http://www.flaticon.com" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a></div>
  
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
    -Switch piece HUD animation.
        Cost: 3
    -Better particle effects for bombs and row bombs.
        Cost: 10
    -Bombs should have bomb meshes.
        Cost: 5
    -Use new GUI/Menu textures.
        Cost: 3
    -Support for iPhone X screen.
        Cost: 10
    -Animations/effects when clearing a level.
        Cost: 7
    -Improving sliding text animation.
        Cost: 3
    -Tutorial.
        Cost: 10
    -Levels/Chapters.
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
    -Camera shake.
        Cost: 3

        Total: 156


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

#include "BlocksApplication.hpp"

#include <iostream>

// Engine includes.
#include "IEngine.hpp"
#include "IInput.hpp"
#include "ISceneManager.hpp"

// Game includes.
#include "UiLayer.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto fadeDuration {0.22f};
    constexpr auto titleFadeInDuration {1.0f};
}

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
    mFadeEffect {engine.GetSceneManager(), engine.GetRenderer(), titleFadeInDuration, 1.0f, 0.0f} {

    engine.GetInput().SetUseGestureRecognizers(false);
    
    mFadeEffect.GetSceneObject().SetLayer(GlobalLayer::sceneSwitchFadeEffect);
    InsertFadeEffectInActiveScene();
    mFadeEffect.StartInMidFade();
}

void BlocksApplication::OnUpdate() {
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
        
        InsertFadeEffectInActiveScene();
    }
}

void BlocksApplication::InsertFadeEffectInActiveScene() {
    auto* scene {mEngine.GetSceneManager().GetActiveScene()};
    scene->GetRoot().AddChild(mFadeEffect.GetSceneObject());
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
