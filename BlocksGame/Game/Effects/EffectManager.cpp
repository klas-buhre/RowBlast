#include "EffectManager.hpp"

using namespace BlocksGame;

namespace {
    constexpr auto numLevelBombEffects {20};
    constexpr auto numLaserEffects {8};
}

EffectManager::EffectManager(Pht::IEngine& engine, GameScene& scene) :
    mExplosionEffect {engine, scene, ExplosionParticleEffect::Kind::Bomb} {
    
    mLaserEffects.resize(numLaserEffects);
    
    for (auto& laser: mLaserEffects) {
        laser = std::make_unique<LaserParticleEffect>(engine, scene);
    }

    mLevelBombExplosionEffects.resize(numLevelBombEffects);
    
    for (auto& levelBombExplosion: mLevelBombExplosionEffects) {
        levelBombExplosion = std::make_unique<ExplosionParticleEffect>(engine,
                                                                       scene,
                                                                       ExplosionParticleEffect::Kind::LevelBomb);
    }
}

void EffectManager::Init() {
    mState = State::Inactive;
    mExplosionEffect.Init();

    for (auto& laser: mLaserEffects) {
        laser->Init();
    }
    
    for (auto& levelBombExplosion: mLevelBombExplosionEffects) {
        levelBombExplosion->Init();
    }
}

void EffectManager::StartExplosion(const Pht::Vec2& position) {
    mState = State::OngoingEffects;
    mExplosionEffect.StartExplosion(position);
}

void EffectManager::StartLaser(const Pht::Vec2& position)  {
    mState = State::OngoingEffects;
    
    for (auto& laser: mLaserEffects) {
        if (laser->GetState() == LaserParticleEffect::State::Inactive) {
            laser->StartLaser(position);
            break;
        }
    }
}

void EffectManager::StartLevelBombExplosion(const Pht::Vec2& position) {
    mState = State::OngoingEffects;
    
    for (auto& levelBombExplosion: mLevelBombExplosionEffects) {
        if (levelBombExplosion->GetState() == ExplosionParticleEffect::State::Inactive) {
            levelBombExplosion->StartExplosion(position);
            break;
        }
    }
}

void EffectManager::Update(float dt) {
    if (mState == State::Inactive) {
        return;
    }

    mExplosionEffect.Update(dt);

    auto anyActiveEffects {false};
    auto onlyParticlesAcitveInLaser {false};

    if (mExplosionEffect.GetState() == ExplosionParticleEffect::State::Ongoing) {
        anyActiveEffects = true;
    }

    for (auto& laser: mLaserEffects) {
        laser->Update(dt);
    
        switch (laser->GetState()) {
            case LaserParticleEffect::State::Ongoing:
                anyActiveEffects = true;
                break;
            case LaserParticleEffect::State::OnlyParticles:
                onlyParticlesAcitveInLaser = true;
                break;
            case LaserParticleEffect::State::Inactive:
                break;
        }
    }

    for (auto& levelBombExplosion: mLevelBombExplosionEffects) {
        levelBombExplosion->Update(dt);
    
        if (levelBombExplosion->GetState() == ExplosionParticleEffect::State::Ongoing) {
            anyActiveEffects = true;
        }
    }

    if (!anyActiveEffects) {
        if (onlyParticlesAcitveInLaser) {
            mState = State::OnlyParticlesActiveInLaser;
        } else {
            mState = State::Inactive;
        }
    }
}
