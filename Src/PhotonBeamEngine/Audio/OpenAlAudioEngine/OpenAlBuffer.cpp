#include "OpenAlBuffer.hpp"

#include <iostream>

#include <OpenAl/alc.h>

#include "AudioFileDecoder.hpp"

using namespace Pht;

namespace {
    ALenum ToOpenAlFormat(int numberOfChannels, int bitsPerChannel) {
        if (numberOfChannels == 1 && bitsPerChannel == 8) {
            return AL_FORMAT_MONO8;
        } else if (numberOfChannels == 1 && bitsPerChannel == 16) {
            return AL_FORMAT_MONO16;
        } if (numberOfChannels == 2 && bitsPerChannel == 8) {
            return AL_FORMAT_STEREO8;
        } else if (numberOfChannels == 2 && bitsPerChannel == 16) {
            return AL_FORMAT_STEREO16;
        } else {
            assert(!"Invalid format.");
        }
    }
}

OpenAlBuffer::OpenAlBuffer(ALuint handle) : mHandle {handle} {}

OpenAlBuffer::~OpenAlBuffer() {
    alDeleteBuffers(1, &mHandle);
}

std::unique_ptr<OpenAlBuffer> Pht::CreateOpenAlBuffer(const std::string& filename) {
    auto audioData {Pht::DecodeAudioFile(filename)};
    
    if (audioData == nullptr) {
        return nullptr;
    }
    
    if (!alcGetCurrentContext()) {
        std::cout << "OpenAlBuffer: ERROR: No context." << std::endl;
        return nullptr;
    }
    
    ALenum error {ALC_NO_ERROR};
    ALuint handle {0};
    
    alGetError();
    alGenBuffers(1, &handle);
    error = alGetError();
    if (error) {
        std::cout << "OpenAlBuffer: ERROR: Could not create OpenAL buffer." << std::endl;
        return nullptr;
    }

    alGetError();
    alBufferData(handle,
                 ToOpenAlFormat(audioData->mNumberOfChannels, audioData->mBitsPerChannel),
                 audioData->mSampleData.data(),
                 static_cast<ALsizei>(audioData->mSampleData.size()),
                 audioData->mSampleRate);
    error = alGetError();
    if (error) {
        std::cout << "OpenAlBuffer: ERROR: Could not copy samples to OpenAL buffer." << std::endl;
        return nullptr;
    }

    return std::make_unique<OpenAlBuffer>(handle);
}
