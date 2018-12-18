#include "IAudioEngine.hpp"

#include <iostream>

#include <OpenAl/al.h>
#include <OpenAl/alc.h>

#include "OpenAlContext.hpp"
#include "OpenAlSound.hpp"

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
        OpenAlAudioEngine() :
            mDevice {std::make_unique<OpenAlDevice>()},
            mContext {std::make_unique<OpenAlContext>(mDevice->GetHandle())} {
            
            mContext->SetIsCurrent(true);
        }
        
        std::unique_ptr<ISound> LoadSound(const std::string& filename, int maxSources) override {
            return OpenAlSound::Create(filename, maxSources);
        }
        
        void SetIsSuspended(bool isSuspended) override {
            if (isSuspended != mContext->IsSuspended()) {
                mContext->SetIsCurrent(isSuspended);
                
                if (isSuspended) {
                    mContext->SetIsCurrent(false);
                } else {
                    mContext->SetIsCurrent(true);
                }
            }
        }
    
        bool IsSuspended() const override {
            return mContext->IsSuspended();
        }

    private:
        std::unique_ptr<OpenAlDevice> mDevice;
        std::unique_ptr<OpenAlContext> mContext;
    };
}

std::unique_ptr<Pht::IAudioEngine> Pht::CreateAudioEngine() {
    return std::make_unique<OpenAlAudioEngine>();
}
