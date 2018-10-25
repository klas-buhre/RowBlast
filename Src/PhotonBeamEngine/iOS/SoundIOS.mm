#include "ISound.hpp"

#include <string>
#include <iostream>

#import <AVFoundation/AVFoundation.h>

@interface AudioPlayer : NSObject {
    AVPlayer* mAvPlayer;
}

- (AVPlayer*) getAvPlayer;
- (void) prepareToPlay: (NSURL*) url;
@end

@implementation AudioPlayer

- (void)prepareToPlay: (NSURL*) url {
    AVAsset* asset = [AVAsset assetWithURL:url];
    NSArray *assetKeys = @[@"playable"];
 
    // Create a new AVPlayerItem with the asset and an
    // array of asset keys to be automatically loaded.
    AVPlayerItem* playerItem = [AVPlayerItem playerItemWithAsset:asset automaticallyLoadedAssetKeys:assetKeys];

    // Associate the player item with the player.
    mAvPlayer = [AVPlayer playerWithPlayerItem:playerItem];
}

- (AVPlayer*) getAvPlayer {
    return mAvPlayer;
}

@end

class SoundIOS: public Pht::ISound {
public:
    SoundIOS(const std::string& filename) {
        NSString* basePath = [NSString stringWithUTF8String:filename.c_str()];
        NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
        NSString* fullPath = [resourcePath stringByAppendingPathComponent:basePath];
        NSURL* soundUrl = [NSURL fileURLWithPath:fullPath];
        
        mAudioPlayer = [AudioPlayer alloc];
        [mAudioPlayer prepareToPlay:soundUrl];
    }
    
    void Play() override {
        std::cout << "-----------Sound----------" << std::endl;
        
        AVPlayer* avPlayer = [mAudioPlayer getAvPlayer];
        [avPlayer seekToTime: CMTimeMake(0, 1)];
        [avPlayer play];
    }
    
    void Pause() override {
        [[mAudioPlayer getAvPlayer] pause];
    }
    
    void Stop() override {
        AVPlayer* avPlayer = [mAudioPlayer getAvPlayer];
        avPlayer.rate = 0.0;
        [avPlayer seekToTime: CMTimeMake(0, 1)];
    }
    
    void SetLoop(bool loop) override {
        // TODO: implement.
    }

private:
    AudioPlayer *mAudioPlayer;
};

std::unique_ptr<Pht::ISound> Pht::LoadSound(const std::string& filename) {
    return std::make_unique<SoundIOS>(filename);
}
