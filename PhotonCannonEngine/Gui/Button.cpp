#include "Button.hpp"

#include "IEngine.hpp"
#include "IRenderer.hpp"
#include "IInput.hpp"
#include "InputEvent.hpp"
#include "SceneObject.hpp"

using namespace Pht;

namespace {
    const Vec4 modelSpaceOrigin {0.0f, 0.0f, 0.0f, 1.0f};
}

Button::Button(SceneObject& sceneObject, const Vec2& size, IEngine& engine) :
    mEngine {engine},
    mSceneObject {sceneObject},
    mSize {size} {}

bool Button::IsClicked(const TouchEvent& event, const Mat4& guiViewTransform) {
    auto result {false};
    auto& renderer {mEngine.GetRenderer()};
    renderer.SetHudMode(true);
    
    switch (OnTouch(event, guiViewTransform)) {
        case Result::Down:
            if (mOnDown) {
                mOnDown();
            }
            break;
        case Result::UpInside:
            if (mOnUpInside) {
                mOnUpInside();
            }
            result = true;
            break;
        case Result::UpOutside:
            if (mOnUpOutside) {
                mOnUpOutside();
            }
            break;
        case Result::MoveOutside:
            if (mOnMoveOutside) {
                mOnMoveOutside();
            }
            break;
        case Result::MoveInside:
        case Result::None:
            break;
    }
    
    renderer.SetHudMode(false);
    return result;
}

bool Button::StateIsDownOrMovedOutside() const {
    switch (mState) {
        case State::Down:
        case State::MovedOutside:
            return true;
        default:
            return false;
    }
}

bool Button::IsDown() const {
    return mState == State::Down;
}

Button::Result Button::OnTouch(const TouchEvent& event, const Mat4& guiViewTransform) {
    auto& touchLocation {event.mLocation};
    
    switch (event.mState) {
        case TouchState::Begin:
            return OnTouchBegin(touchLocation, guiViewTransform);
        case TouchState::Ongoing:
            return OnTouchMove(touchLocation, guiViewTransform);
        case TouchState::End:
            return OnTouchEnd(touchLocation, guiViewTransform);
        case TouchState::Other:
            break;
    }

    return Result::None;
}

Button::Result Button::OnTouchBegin(const Vec2& touchLocation, const Mat4& transform) {
    switch (mState) {
        case State::Up:
            if (Hit(touchLocation, transform)) {
                mState = State::Down;
                return Result::Down;
            }
            break;
        case State::Down:
        case State::MovedOutside:
            break;
    }
    
    return Result::None;
}

Button::Result Button::OnTouchMove(const Vec2& touchLocation, const Mat4& transform) {
    switch (mState) {
        case State::Up:
            return Result::None;
        case State::Down:
            if (Hit(touchLocation, transform)) {
                return Result::MoveInside;
            } else {
                mState = State::MovedOutside;
                return Result::MoveOutside;
            }
            break;
        case State::MovedOutside:
            if (Hit(touchLocation, transform)) {
                mState = State::Down;
                return Result::Down;
            }
            break;
    }
    
    return Result::None;
}

Button::Result Button::OnTouchEnd(const Vec2& touchLocation, const Mat4& transform) {
    auto result {Result::None};

    switch (mState) {
        case State::Up:
            break;
        case State::MovedOutside:
            if (Hit(touchLocation, transform)) {
                result = Result::UpInside;
            } else {
                result = Result::UpOutside;
            }
            break;
        case State::Down:
            if (Hit(touchLocation, transform)) {
                result = Result::UpInside;
            }
            break;
    }
    
    mState = State::Up;
    return result;
}

bool Button::Hit(const Vec2& touch, const Mat4& transform) {
    auto& renderer {mEngine.GetRenderer()};
    auto modelView {(mSceneObject.GetMatrix() * transform) * renderer.GetViewMatrix()};
    auto normProjPos {renderer.GetProjectionMatrix() * modelView.Transposed() * modelSpaceOrigin};
    
    auto& screenInputSize {mEngine.GetInput().GetScreenInputSize()};
    
    Vec2 buttonPos {
        (normProjPos.x + 1.0f) / 2.0f * screenInputSize.x,
        (-normProjPos.y + 1.0f) / 2.0f * screenInputSize.y
    };
    
    auto halfSizeX {mSize.x / 2.0f};
    auto halfSizeY {mSize.y / 2.0f};
    
    return touch.x > buttonPos.x - halfSizeX && touch.x < buttonPos.x + halfSizeX &&
           touch.y > buttonPos.y - halfSizeY && touch.y < buttonPos.y + halfSizeY;
}
