/*
Backlog:
  -Engine:
    -Physics.
    -Purchases.
    -Net/HTTP.
    -Analytics.
    
  -Gameplay:
    -Prevent the crossed tilted welds from happening seven and mirrored seven (in both
     GestureInputHandler and ClickInputHandler) so that those pieces can be pulled down. Or, allow
     it and solve it when pulling down loose pieces?
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
    -Slightly lighter blue in the field quad?
    -Could try reducing scroll speed in map scene using a spring.
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
    -Could have particles in the FlyingBlocksAnimation in the points from which the blocks gets
     tossed.
    -The blocks in FlyingBlocksAnimation should bounce off each other instead of passing right 
     through.
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
  
Ongoing tasks:
  -Slightly lighter blue in the field quad?
  -Avoid discontinuous derivative after pulling down loose pieces:
    -Tune the following parameters:
        gravitationalAcceleration {-35.0f};
        springCoefficient {2000.0f};
        dampingCoefficient {40.0f};
        waitTime {0.15f};
    -Thoughts:
        -Options:
          -Maybe the blocks could be compressed a bit.
          -Maybe the blocks could bounce.
        -Try small bounce for pulled down pieces and a very small compress. Maybe no need to change the
         y scale, could just let the pulled down block pass into the lower blocks with about 10% ? That
         would only look good if the z value of the pulled down block (except the tilted welds) was
         moved back.
        -Should level blocks bounce if they fall down on other field blocks?
          -It may look better if they do, so yes.
          -If they bounce an algorithm need to find level blocks that hit (and all connected blocks)
           other field blocks.
        -Level blocks that do not fall down on other field blocks (floating blocks) should not bounce.
        -Non-level blocks that sit on top of level blocks that fall but not bouncing. Should those non-
         level blocks bounce?
          -They should not.
    -Decided:
        -Try small bounce for pulled down pieces and a very small fall through (start with the
         bounce since compression requires knowing which blocks to compress and solving the
         problem of the tilted weld).
        -All falling blocks should bounce except level blocks (and the blocks sitting on top of
         those level blocks) that do not fall down on other field blocks.
    -Questions:
        -How to detect which blocks should bounce?
        -How to do the bounce? Damped springs?
    -Algorithm for detecting which blocks should bounce based on removed rows (algorithm #1):
       -Run it before the pulling down of loose pieces since algorithm #2 depends on
        this algorithm, so maybe during RemoveRowImpl (should not be needed after
        RemoveAreaOfSubCells).
        RemoveRowImpl(rowIndex):
            DetectBlocksThatShouldBounce(rowIndex)
            Remove row...
        end
 
        DetectBlocksThatShouldBounce(removedRowIndex)
            for each column
                upperBlock = mGrid[removedRowIndex + 1][column]
                if rowIndex - 1 < mLowestVisibleRow || !mGrid[removedRowIndex - 1][column].isEmpty
                    if !upperBlock.isEmpty
                        if upperBlock.isLevel
                            SetShouldBounceLevelBlock(removedRowIndex + 1, upperBlock)
                        else
                            firstSubCell = upperBlock.mFirstSubCell
                            SetShouldBouncePiece(removedRowIndex + 1, firstSubCell.mColor, firstSubCell)
                            secondSubCell = upperBlock.mSecondSubCell
                            SetShouldBouncePiece(removedRowIndex + 1, secondSubCell.mColor, secondSubCell)
                        end
                    end
                end
            end
        end
 
        SetShouldBounceLevelBlock(lowestSearchRow, block)
            if IsOutsideField(block) ||
               block.mShouldBounce ||
               block.y < lowestSearchRow ||
               !block.isLevel ||
               block.isEmpty
                return
            end
            block.mShouldBounce = true
            SetShouldBounceLevelBlock(lowestSearchRow, left)
            SetShouldBounceLevelBlock(lowestSearchRow, right)
            SetShouldBounceLevelBlock(lowestSearchRow, up)
            SetShouldBounceLevelBlock(lowestSearchRow, down)
        end
 
        SetShouldBouncePiece(lowestSearchRow, block, color)
            // The checking of colors and sub cells should work more or less as Field::FindPieceBlocks
            if block.mShouldBounce || block.y < lowestSearchRow
                return
            end
            block.mShouldBounce = true
            if block.welds.up
                SetShouldBouncePiece(lowestSearchRow, upperBlock, color)
            end
            if block.welds.upRight
                SetShouldBouncePiece(lowestSearchRow, upperRightBlock, color)
            end
            if block.welds.left
                SetShouldBouncePiece(lowestSearchRow, leftBlock, color)
            end
            ...
        end
 
    -Algorithm for detecting which non-level blocks should bounce (algorithm #2):
        During pull down of loose pieces:
            if ShouldPieceBounce(piecePosition, mCollisionResult, piece)
                SetShouldBounce(pieceBlocks)
            end
 
        ShouldPieceBounce(piecePosition, collisionResult, piece)
            if piecPositio.y > collisionPosition.y + 1
                // Piece blocks will fall through empty space.
                return true
            end
            // If some of the blocks holding up the piece should bounce, then the piece must bounce.
            switch collisionResult.mIsCollison
                case Collision
                    return AnyBlocksShouldBounce(collisionResult.mCollisionPoints + piecePosition))
                case NextWillBe
                    return AnyBlocksShouldBounce(collisionResult.mNextCollisionPoints + piecePosition))
                    break
                case NoCollision
                    assert
                    break
            end
            assert
            return false
        end
 
        Field::CheckCollision()
            result.mNextCollisionPoints.Clear();
            ...
            if (firstSubCellIntersects == Intersection::NextWillBe ||
                secondSubCellIntersects == Intersection::NextWillBe) {
                result.mNextCollisionPoints.PushBack(Pht::IVec2{pieceColumn, pieceRow});
            }
            ...
            if (result.mCollisionPoints.Size() > 0) {
                result.mIsCollision = IsCollision::Yes;
            } else if (result.mNextmCollisionPoints.Size() > 0) {
                result.mIsCollision = IsCollision::NextWillBe;
            }
        end

----------------------------------------------------------------------------------------------------
1:
Both B and Y should bounce because they fall and land.

1.1:
    7  YY
    6 BBY
    5 BBB  YY
    4 GGGGGYG
    3     G
    2     G
    1 GGGGG

1.2:
    7
    6  YY
    5 BBY
    4 BBB  YY
    3     G
    2     G
    1 GGGGG

1.3:
    7
    6
    5
    4  YY
    3 BBY G
    2 BBB G
    1 GGGGGYY
 
----------------------------------------------------------------------------------------------------
2:
Row 4->2 level blocks should bounce because they fall and land and both B and Y should bounce
too because they sit on top of those level blocks.

2.1:
    7  YY
    6 BBY
    5 BBB
    4 GGGGG B
    3 GGGGGBG
    2 GGGGGBG
    1 GGGGG G

2.2:
    7
    6
    5  YY
    4 BBY
    3 BBB
    2 GGGGG B
    1 GGGGG G

----------------------------------------------------------------------------------------------------
3:
Both B and Y should not bounce because they sit on top of row 4 of level backs that don't fall and
land because thay are floating.

3.1:
    7  YY
    6 BBY
    5 BBB
    4 GGGGG
    3
    2 GGGGGRG
    1 GGG

3.2:
    7
    6  YY
    5 BBY
    4 BBB
    3 GGGGG
    2
    1 GGG

----------------------------------------------------------------------------------------------------
4:
Both B and Y should probably bounce even though they sit partly on top of the floating level block.
The other level blocks in row 4 and 3 should bounce since they fall and land and because of that B
and Y need to bounce, otherwise the level blocks would bounce up through B.

4.1:
    7  YY
    6 BBY
    5 BBB
    4 G GGG
    3   G
    2 GGGGGRG
    1 GGG

4.2:
    7
    6  YY
    5 BBY
    4 BBB
    3 G GGG
    2   G
    1 GGG  R

----------------------------------------------------------------------------------------------------
5: A problem in this scenario could be that it could be tricky to calc the collision between row 3
and row 7 if row 3 have not finished bouncing when row 7 lands on it.

5.1:
    9    B      9    b
    8 GGBB      8 bbbb
    7 GGBB      7 bbbb
    6 GGGGGYG   6
    5 GGGGGYG   5
    4 GGGGGYG   4
    3 GGGG Y    3 bbbb
    2 GGGGGYG   2
    1 GGGG  G   1 GGGG bG

5.2:
    9
    8
    7
    6
    5    B
    4 GGBB
    3 GGBB
    2 GGGG
    1 GGGG YG

----------------------------------------------------------------------------------------------------
6:
B should bounce because row 2 and 3 are removed. Y should bounce beacause it sits on top of B.

6.1:
    6  YY
    5 BBY
    4 BBB
    3 GGGGGBG
    2 GGGGGBG
    1 GGGGG G

6.2:
    6
    5
    4  YY
    3 BBY
    2 BBB
    1 GGGGG G

----------------------------------------------------------------------------------------------------

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
    -Block compression and/or bounce.
        Cost: 5
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
    -Moving the next level particle effect to the next level after clearing a level.
        Cost: 2
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

        Total: 166


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
    mEngine.GetInput().DisableInput();
}

void BlocksApplication::BeginFadeToGame(int level) {
    mLevelToStart = level;
    mFadeEffect.Start();
    mNextState = State::GameScene;
    mEngine.GetInput().DisableInput();
}

void BlocksApplication::StartMap() {
    mState = State::MapScene;
    mMapController.Init();
    mEngine.GetInput().EnableInput();
}

void BlocksApplication::StartGame() {
    mState = State::GameScene;
    mGameController.StartLevel(mLevelToStart);
}
