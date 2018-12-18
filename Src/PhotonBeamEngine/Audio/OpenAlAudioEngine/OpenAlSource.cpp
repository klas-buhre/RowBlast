#include "OpenAlSource.hpp"

#include <iostream>

using namespace Pht;

OpenAlSource::OpenAlSource(ALuint handle) : mHandle {handle} {}

OpenAlSource::~OpenAlSource() {
    if (IsPlaying()) {
        Stop();
    }
    
    alSourcei(mHandle, AL_BUFFER, 0 /* detach */);
    alDeleteSources(1, &mHandle);
}

void OpenAlSource::Play() {
    if (IsPlaying()) {
        Stop();
    }
    
    alSourcePlay(mHandle);
}

void OpenAlSource::Stop() {
    if (IsPlaying()) {
        alSourceStop(mHandle);
    }
}

bool OpenAlSource::IsPlaying() const {
    ALint state;
    alGetSourcei(mHandle, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

void OpenAlSource::SetGain(float gain) {
    alSourcef(mHandle, AL_GAIN, gain);
}

void OpenAlSource::SetPitch(float pitch) {
    alSourcef(mHandle, AL_PITCH, pitch);
}

void OpenAlSource::SetLoop(bool loop) {
    alSourcei(mHandle, AL_LOOPING, loop);
}

std::unique_ptr<OpenAlSource> OpenAlSource::Create(const OpenAlBuffer& buffer) {
    ALuint handle {0};
    
    alGetError();
    alGenSources(1, &handle);
    alSourcei(handle, AL_BUFFER, buffer.GetHandle());
    ALenum error {alGetError()};
    if (error) {
        std::cout << "OpenAlBuffer: ERROR: Could not create OpenAL source." << std::endl;
        return nullptr;
    }
    
    return std::make_unique<OpenAlSource>(handle);
}
