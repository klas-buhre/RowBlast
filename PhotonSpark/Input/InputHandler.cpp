#include "InputHandler.hpp"

#include <assert.h>

using namespace Pht;

namespace {
    const auto eventQueueMaxSize {1000};
    const Vec2 defaultScreenInputSize {320.0f, 568.0f};
}

InputHandler::InputHandler(const Vec2& nativeScreenInputSize) :
    mNativeScreenInputSize {nativeScreenInputSize} {
    
    auto nativeWhRatio {nativeScreenInputSize.x / nativeScreenInputSize.y};
    auto defaultWhRatio {defaultScreenInputSize.x / defaultScreenInputSize.y};
    
    mScreenInputSize.x = nativeWhRatio * defaultScreenInputSize.x / defaultWhRatio;
    mScreenInputSize.y = defaultScreenInputSize.y;
    
    mEventQueue.reserve(eventQueueMaxSize);
}

void InputHandler::SetUseGestureRecognizers(bool useGestureRecognizers) {
    mUseGestureRecognizers = useGestureRecognizers;
}

void InputHandler::PushToQueue(InputEvent& event) {
    ProcessInputEvent(event);

    if (mEventQueue.size() < eventQueueMaxSize) {
        mEventQueue.push_back(event);
    } else {
        mEventQueue[mQueueWriteIndex % eventQueueMaxSize] = event;
    }
    
    ++mQueueWriteIndex;
    assert(mQueueWriteIndex - mQueueReadIndex <= eventQueueMaxSize);
}

bool InputHandler::HasEvents() const {
    return mQueueReadIndex != mQueueWriteIndex;
}

const InputEvent& InputHandler::GetNextEvent() const {
    return mEventQueue[mQueueReadIndex % eventQueueMaxSize];
}

void InputHandler::PopNextEvent() {
    ++mQueueReadIndex;
}

bool InputHandler::ConsumeWholeTouch() {
    auto gotTouch {false};
    
    while (HasEvents()) {
        auto& event {GetNextEvent()};
        switch (event.GetKind()) {
            case Pht::InputKind::Touch:
                if (event.GetTouchEvent().mState == Pht::TouchState::End) {
                    gotTouch = true;
                }
                break;
            default:
                assert(false);
                break;
        }
        
        PopNextEvent();
        
        if (gotTouch) {
            break;
        }
    }
    
    return gotTouch;
}

const Vec2& InputHandler::GetScreenInputSize() const {
    return mScreenInputSize;
}

void InputHandler::ProcessInputEvent(InputEvent& event) {
    switch (event.GetKind()) {
        case InputKind::Touch: {
            auto& touchEvent {event.mTouch};
            touchEvent.mLocation = NativeToStandardCoordinates(touchEvent.mLocation);
            ProcessTouchEvent(touchEvent);
            break;
        }
        case InputKind::TapGesture: {
            auto& tapEvent {event.mTap};
            tapEvent.mLocation = NativeToStandardCoordinates(tapEvent.mLocation);
            break;
        }
        case InputKind::PanGesture: {
            auto& panEvent {event.mPan};
            panEvent.mTranslation = NativeToStandardCoordinates(panEvent.mTranslation);
            break;
        }
    }
}

void InputHandler::ProcessTouchEvent(TouchEvent& event) {
    switch (event.mState) {
        case TouchState::Begin:
            mTouchBeginLocation = event.mLocation;
            event.mTranslation = {0.0f, 0.0f};
            event.mVelocity = {0.0f, 0.0f};
            break;
        case TouchState::Ongoing:
            event.mTranslation = event.mLocation - mTouchBeginLocation;
            event.mVelocity = event.mLocation - mTouchPreviousLocation;
            break;
        case TouchState::End:
            event.mTranslation = event.mLocation - mTouchBeginLocation;
            if (event.mLocation == mTouchPreviousLocation) {
                event.mVelocity = mPreviousVelocity;
            } else {
                event.mVelocity = event.mLocation - mTouchPreviousLocation;
            }
            break;
        default:
            break;
    }
    
    mTouchPreviousLocation = event.mLocation;
    mPreviousVelocity = event.mVelocity;
}

Vec2 InputHandler::NativeToStandardCoordinates(const Vec2& nativeLocation) {
    return {
        nativeLocation.x * mScreenInputSize.x / mNativeScreenInputSize.x,
        nativeLocation.y * mScreenInputSize.y / mNativeScreenInputSize.y
    };
}
