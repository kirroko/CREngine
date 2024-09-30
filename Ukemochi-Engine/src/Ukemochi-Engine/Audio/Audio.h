#ifndef AUDIO_H
#define AUDIO_H

#include <fmod.hpp>
#include <iostream>
#include <vector>
#include "../ECS/Systems.h"

namespace Ukemochi
{
    class Audio : public System
    {
    public:
        static Audio& GetInstance()
        {
            static std::unique_ptr<Audio> instance(new Audio());
            return *instance;
        }
        Audio();
        ~Audio();

        void CreateGroup(const char* groupname);
        bool LoadSound(const char* filePath);
        void PlaySoundInGroup(int soundIndex, int groupIndex);
        void StopSound(int soundIndex);
        void ToggleSoundInGroup(int soundIndex, int groupIndex);
        void SetGroupVolume(int groupIndex, float volume);
        void StopAllSoundsInGroup(int groupIndex);
        void Update();

    private:
        FMOD::System* pSystem;
        std::vector<FMOD::ChannelGroup*> pChannelGroups;
        std::vector<FMOD::Sound*> pSounds;
        std::vector<FMOD::Channel*> pChannels;
        int numOfAudios;
    };
}

#endif // AUDIO_H
