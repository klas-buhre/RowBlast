#include "OpenAlContext.hpp"

#include <iostream>

using namespace Pht;

OpenAlContext::OpenAlContext(ALCdevice *device) {
    if (device) {
        mHandle = alcCreateContext(device, 0);
    }
    
    if (mHandle == nullptr) {
        std::cout << "OpenAlContext: ERROR: Could not create context." << std::endl;
    }
}

OpenAlContext::~OpenAlContext() {
    if (mHandle) {
        if (IsCurrent()) {
            SetIsCurrent(false);
        }
        
        alcDestroyContext(mHandle);
    }
}

void OpenAlContext::SetIsCurrent(bool isCurrent) {
    alcMakeContextCurrent(isCurrent ? mHandle : nullptr);
}

bool OpenAlContext::IsCurrent() const {
    return mHandle && (alcGetCurrentContext() == mHandle);
}

void OpenAlContext::SetIsSuspended(bool isSuspended) {
    if (mHandle && (isSuspended != mIsSuspended)) {
        if (isSuspended) {
            alcSuspendContext(mHandle);
        } else {
            alcProcessContext(mHandle);
        }
        
        mIsSuspended = isSuspended;
    }
}
