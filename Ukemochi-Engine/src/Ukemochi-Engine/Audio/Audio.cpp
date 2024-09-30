
#include "PreCompile.h"
#include "Audio.h"

namespace Ukemochi
{
    Audio::Audio()
        : pSystem(nullptr), numOfAudios(0)
    {
        FMOD_RESULT result;

        // Create FMOD system
        result = FMOD::System_Create(&pSystem);
        if (result != FMOD_OK)
        {
            std::cerr << "FMOD system creation failed: " << result << std::endl;
            return;
        }

        // Initialize the system
        result = pSystem->init(32, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            std::cerr << "FMOD system initialization failed: " << result << std::endl;
            return;
        }
        CreateGroup();
    }


    Audio::~Audio()
    {
        // Release all sounds
        for (auto sound : pSounds)
        {
            if (sound)
            {
                sound->release();
                sound = nullptr;
            }
        }
        pSounds.clear();

        // Release all channel groups
        for (auto group : pChannelGroups)
        {
            if (group)
            {
                group->release();
                group = nullptr;
            }
        }
        pChannelGroups.clear();

        for (auto chanel : pChannels)
        {
            if (chanel)
            {
                chanel = nullptr;
            }
        }
        pChannels.clear();

        // Release FMOD system
        if (pSystem)
        {
            pSystem = nullptr;
        }

        numOfAudios = 0;
    }

    void Audio::CreateGroup()
    {
        FMOD::ChannelGroup* group = nullptr;
        FMOD_RESULT result;

        for (int i = 0; i < LAST; i++)
        {
            ChannelGroups channel = static_cast<ChannelGroups>(i);
            switch (channel)
            {
            case Ukemochi::ENGINEAUDIO:
                result = pSystem->createChannelGroup("ENGINE", &group);
                break;
            case Ukemochi::MENUAUDIO:
                result = pSystem->createChannelGroup("MENU", &group);
                break;
            case Ukemochi::LEVEL1:
                result = pSystem->createChannelGroup("LEVEL1", &group);
                break;
            case Ukemochi::LAST:
                break;
            default:
                break;
            }

            if (result != FMOD_OK)
            {
                std::cerr << "Failed to create music channel group: " << result << std::endl;
                return;
            }
            pChannelGroups.push_back(group);
        }
    }

    bool Audio::LoadSound(const char* filePath)
    {
        FMOD::Sound* sound = nullptr;
        FMOD_RESULT result;

        // Load the sound
        result = pSystem->createSound(filePath, FMOD_DEFAULT, nullptr, &sound);
        if (result != FMOD_OK)
        {
            std::cerr << "Failed to load sound: " << result << std::endl;
            return false;
        }

        // Add the sound to the vector of sounds
        pSounds.push_back(sound);
        pChannels.push_back(nullptr);  // Add a corresponding channel for each sound
        ++numOfAudios;

        return true;
    }

    void Audio::PlaySoundInGroup(int soundIndex, int groupIndex)
    {
        if (soundIndex < numOfAudios && groupIndex < pChannelGroups.size())
        {
            FMOD_RESULT result;
            FMOD::Channel* channel = nullptr;

            // Play the sound
            result = pSystem->playSound(pSounds[soundIndex], nullptr, false, &channel);
            if (result != FMOD_OK)
            {
                std::cerr << "Failed to play sound: " << result << std::endl;
                return;
            }

            // Store the channel and assign it to the specific group
            pChannels[soundIndex] = channel;
            pChannels[soundIndex]->setChannelGroup(pChannelGroups[groupIndex]);

            std::cout << "Sound " << soundIndex << " is playing in group " << groupIndex << std::endl;
        }
        else
        {
            std::cerr << "Invalid sound or group index!" << std::endl;
        }
    }

    void Audio::StopSound(int soundIndex)
    {
        if (soundIndex < numOfAudios && pChannels[soundIndex] != nullptr)
        {
            bool isPlaying = false;
            pChannels[soundIndex]->isPlaying(&isPlaying);

            if (isPlaying)
            {
                pChannels[soundIndex]->stop();  // Stop the sound if it's playing
            }
        }
    }

    void Audio::ToggleSoundInGroup(int soundIndex, int groupIndex)
    {
        if (soundIndex < numOfAudios && groupIndex < pChannelGroups.size())
        {
            bool isPlaying = false;
            if (pChannels[soundIndex] != nullptr)
            {
                pChannels[soundIndex]->isPlaying(&isPlaying);
            }

            if (isPlaying)
            {
                StopSound(soundIndex);  // Stop the sound if it's currently playing
            }
            else
            {
                PlaySoundInGroup(soundIndex, groupIndex);  // Play the sound in the specified group if it's not playing
            }
        }
        else
        {
            std::cerr << "Invalid sound or group index!" << std::endl;
        }
    }

    void Audio::SetAudioVolume(int soundIndex, float volume)
    {
        if (soundIndex < pChannelGroups.size())
        {
            pChannels[soundIndex]->setVolume(volume);
        }
    }

    void Audio::SetGroupVolume(int groupIndex, float volume)
    {
        if (groupIndex < pChannelGroups.size())
        {
            pChannelGroups[groupIndex]->setVolume(volume);  // Set the volume for the specific group
        }
    }

    void Audio::StopAllSoundsInGroup(int groupIndex)
    {
        if (groupIndex < pChannelGroups.size())
        {
            pChannelGroups[groupIndex]->stop();  // Stop all sounds in the specified group
        }
    }

    void Audio::Update()
    {
        // Update the FMOD system regularly
        pSystem->update();
    }
    bool Audio::IsPlaying(int soundIndex)
    {
        bool isPlaying = false;
        if (soundIndex < numOfAudios && pChannels[soundIndex] != nullptr)
        {
            pChannels[soundIndex]->isPlaying(&isPlaying);
        }
        return isPlaying;
    }
}
