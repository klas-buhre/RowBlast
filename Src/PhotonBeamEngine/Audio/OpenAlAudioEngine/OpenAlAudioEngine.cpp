#include "IAudioEngine.hpp"

#include <iostream>

#include <OpenAl/al.h>
#include <OpenAl/alc.h>

using namespace Pht;

namespace {
    class OpenAlDevice {
    public:
        OpenAlDevice() : mHandle {alcOpenDevice(nullptr)} {
            if (mHandle == nullptr) {
                std::cout << "OpenAlDevice: ERROR: Could not open device." << std::endl;
            }
        }
        
        ~OpenAlDevice() {
            if (mHandle) {
                alcCloseDevice(mHandle);
            }
        }
        
        ALCdevice* GetHandle() {
            return mHandle;
        }
        
    private:
        ALCdevice* mHandle {nullptr};
    };
    
    class OpenAlAudioEngine: public IAudioEngine {
    public:
        OpenAlAudioEngine() {
            mDevice = std::make_unique<OpenAlDevice>();
            
            
        }
        
        std::unique_ptr<ISound> LoadSound(const std::string& filename, int maxPolyphony) override {
        
        }
        
        void SetIsSuspended(bool isSuspended) override {
        
        }
        
        bool IsSuspended() const override {
        
        }

    private:
        std::unique_ptr<OpenAlDevice> mDevice;
    };
}

std::unique_ptr<Pht::IAudioEngine> Pht::CreateAudioEngine() {
    return std::make_unique<OpenAlAudioEngine>();
}
