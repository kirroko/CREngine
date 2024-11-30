/* Start Header ************************************************************************/
/*!
\file       Audio.cpp
\author     Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu
\date       Oct 4, 2024
\brief      This file contains the definition of the Audio system.

This Audio system handles loading, playing, and managing sound effects
and background music using FMOD. It includes sound groups for
controlling multiple audio sources together.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Audio.h"

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Constructor for the Audio class.
     This is where system initialization and resource allocation happen.
    *************************************************************************/
    Audio::Audio()
        : pSystem(nullptr), numOfMusic(0), numOfSFX(0)
    {
        FMOD_RESULT result;

        // Create FMOD system
        result = FMOD::System_Create(&pSystem);
        if (result != FMOD_OK)
        {
            std::cerr << "FMOD system creation failed: " << result << std::endl;
            return;
        }

        // Initialize the FMOD system with 32 channels
        result = pSystem->init(32, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            std::cerr << "FMOD system initialization failed: " << result << std::endl;
            return;
        }
        CreateGroup();
    }

    /*!***********************************************************************
    \brief
     Destructor for the Audio class.
     This is where system cleanup and resource deallocation happen.
    *************************************************************************/
    Audio::~Audio()
    {
        // Release all sounds
        for (auto sound : pSFX)
        {
            if (sound)
            {
                sound->release();
                sound = nullptr;
            }
        }
        pSFX.clear();

        for (auto sound : pMusic)
        {
            if (sound)
            {
                sound->release();
                sound = nullptr;
            }
        }
        pMusic.clear();

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

        for (auto chanel : pSFXChannels)
        {
            if (chanel)
            {
                chanel = nullptr;
            }
        }
        pSFXChannels.clear();

        for (auto chanel : pMusicChannels)
        {
            if (chanel)
            {
                chanel = nullptr;
            }
        }
        pMusicChannels.clear();

        // Release FMOD system
        if (pSystem)
        {
            pSystem = nullptr;
        }

        numOfMusic = numOfSFX = 0;
    }

    /*!***********************************************************************
    \brief
        Create a new ChannelGroup for managing sound channels.
        ChannelGroups help manage a group of channels as one unit.
    *************************************************************************/
    void Audio::CreateGroup()
    {
        FMOD::ChannelGroup* group = nullptr;
        FMOD_RESULT result = FMOD_ERR_UNINITIALIZED;

        // Loop through all predefined channel groups
        for (int i = 0; i < LAST; i++)
        {
            ChannelGroups channel = static_cast<ChannelGroups>(i);

            // Create different groups based on their enum value
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

            // Add the group to the vector
            pChannelGroups.push_back(group);
        }
    }

    /*!***********************************************************************
    \brief
        Load a sound from a file.
        \param filePath: The file path to the sound file to be loaded.
        \return True if the sound was successfully loaded, false otherwise.
    *************************************************************************/
    bool Audio::LoadSound(int index, const char* filePath ,std::string type)
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
        if (type == "SFX")
        {
            // Add the sound to the vector of sounds
            if (pSFX.empty()||index == pSFX.size())
            {
                pSFX.push_back(sound);
                pSFXChannels.push_back(nullptr);  // Add a corresponding channel for each sound
                ++numOfSFX;
            }
            else
            {
                pSFX[index] = sound;
            }

        }
        else if (type == "Music")
        {
            // Add the sound to the vector of sounds
            if (pMusic.empty() || index == pMusic.size())
            {
                pMusic.push_back(sound);
                pMusicChannels.push_back(nullptr);  // Add a corresponding channel for each sound
                ++numOfMusic;
            }
            else
            {
                pMusic[index] = sound;
            }
        }



        return true;
    }

    void Audio::PlaySound(int soundIndex, std::string type)
    {
        if (soundIndex < pSFX.size() || soundIndex < pMusic.size())

        {
            FMOD_RESULT result;
            FMOD::Channel* channel = nullptr;

            if (type == "SFX")
            {
                // Play the sound
                result = pSystem->playSound(pSFX[soundIndex], nullptr, false, &channel);
                if (result != FMOD_OK)
                {
                    std::cerr << "Failed to play sound: " << result << std::endl;
                    return;
                }

                // Store the channel and assign it to the specific group
                pSFXChannels[soundIndex] = channel;
                pSFXChannels[soundIndex]->setVolume(0.2f);

                std::cout << "Sound " << soundIndex << " is playing in group " << soundIndex << std::endl;
            }
            else if (type == "Music")
            {
                result = pMusic[soundIndex]->setMode(FMOD_LOOP_NORMAL);
                if (result != FMOD_OK) {
                    std::cerr << "Failed to set loop mode: " << result << std::endl;
                    return;
                }

                // Set the loop count (-1 for infinite looping)
                result = pMusic[soundIndex]->setLoopCount(-1);

                // Play the sound
                result = pSystem->playSound(pMusic[soundIndex], nullptr, false, &channel);
                if (result != FMOD_OK)
                {
                    std::cerr << "Failed to play sound: " << result << std::endl;
                    return;
                }

                // Store the channel and assign it to the specific group
                pMusicChannels[soundIndex] = channel;
                pMusicChannels[soundIndex]->setVolume(0.2f);
                std::cout << "Sound " << soundIndex << " is playing in group " << soundIndex << std::endl;

            }
        }
        else
        {
            std::cerr << "Invalid sound or group index!" << std::endl;
        }
    }

    /*!***********************************************************************
    \brief
    Play a sound in a specified group.
    \param soundIndex: Index of the sound to play.
    \param groupIndex: Index of the group in which the sound should be played.
    *************************************************************************/
    //void Audio::PlaySoundInGroup(int soundIndex, int groupIndex)
    //{
    //    if (soundIndex < numOfAudios && groupIndex < pChannelGroups.size())
    //    {
    //        FMOD_RESULT result;
    //        FMOD::Channel* channel = nullptr;

    //        // Play the sound
    //        result = pSystem->playSound(pSounds[soundIndex], nullptr, false, &channel);
    //        if (result != FMOD_OK)
    //        {
    //            std::cerr << "Failed to play sound: " << result << std::endl;
    //            return;
    //        }

    //        // Store the channel and assign it to the specific group
    //        pChannels[soundIndex] = channel;
    //        pChannels[soundIndex]->setChannelGroup(pChannelGroups[groupIndex]);

    //        std::cout << "Sound " << soundIndex << " is playing in group " << groupIndex << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "Invalid sound or group index!" << std::endl;
    //    }
    //}

    /*!***********************************************************************
    \brief
    Stop playing a specific sound.
    \param soundIndex: Index of the sound to stop.
    *************************************************************************/

    void Audio::StopSound(int soundIndex, std::string type)
    {
        if (type == "SFX")
        {
            if (soundIndex < numOfSFX && pSFX[soundIndex] != nullptr)
            {
                bool isPlaying = false;
                pSFXChannels[soundIndex]->isPlaying(&isPlaying);

                if (isPlaying)
                {
                    pSFXChannels[soundIndex]->stop();  // Stop the sound if it's playing
                }
            }
        }
        else if (type == "Music")
        {
            if (soundIndex < numOfMusic && pMusic[soundIndex] != nullptr)
            {
                bool isPlaying = false;
                pMusicChannels[soundIndex]->isPlaying(&isPlaying);

                if (isPlaying)
                {
                    pMusicChannels[soundIndex]->stop();  // Stop the sound if it's playing
                }
            }
        }
    }

    void Audio::StopAllSound()
    {
        for (auto* channel : pSFXChannels)
        {
            channel->stop();
        }
        for (auto* channel : pMusicChannels)
        {
            channel->stop();
        }
    }

    void Audio::PlayGameBGM()
    {
        bool isPlaying = false;
        pMusicChannels[0]->isPlaying(&isPlaying);

        if (!isPlaying)
        {
            PlaySound(0, "Music");
        }
    }

    /*!***********************************************************************
    \brief
    Toggle a sound in a group (play or pause).
    \param soundIndex: Index of the sound to toggle.
    \param groupIndex: Index of the group where the sound resides.
    *************************************************************************/
    //void Audio::ToggleSoundInGroup(int soundIndex, int groupIndex)
    //{
    //    if (soundIndex < numOfAudios && groupIndex < pChannelGroups.size())
    //    {
    //        bool isPlaying = false;

    //        if (pChannels[soundIndex] != nullptr)
    //        {
    //            pChannels[soundIndex]->isPlaying(&isPlaying);
    //        }

    //        if (isPlaying)
    //        {
    //            StopSound(soundIndex);  // Stop the sound if it's currently playing
    //        }
    //        else
    //        {
    //            PlaySoundInGroup(soundIndex, groupIndex);  // Play the sound in the specified group if it's not playing
    //        }
    //    }
    //    else
    //    {
    //        std::cerr << "Invalid sound or group index!" << std::endl;
    //    }
    //}

    /*!***********************************************************************
    \brief
    Set the volume for a specific sound.
    \param soundIndex: Index of the sound whose volume is being set.
    \param volume: The volume level (0.0 to 1.0).
    *************************************************************************/
    void Audio::SetAudioVolume(int soundIndex, float volume, std::string type)
    {
        if (type == "SFX")
        {
            if (soundIndex < pSFXChannels.size() && pSFXChannels[soundIndex] != nullptr)
            {
                pSFXChannels[soundIndex]->setVolume(volume);
            }
        }
        else if (type == "Music")
        {
            if (soundIndex < pMusicChannels.size() && pMusicChannels[soundIndex] != nullptr)
            {
                pMusicChannels[soundIndex]->setVolume(volume);
            }
        }

    }

    /*!***********************************************************************
    \brief
    Set the volume for an entire group of sounds.
    \param groupIndex: Index of the group whose volume is being set.
    \param volume: The volume level (0.0 to 1.0).
    *************************************************************************/
    void Audio::SetGroupVolume(int groupIndex, float volume)
    {
        if (groupIndex < pChannelGroups.size() && pChannelGroups[groupIndex] != nullptr)
        {
            pChannelGroups[groupIndex]->setVolume(volume);  // Set the volume for the specific group
        }
    }

    /*!***********************************************************************
    \brief
    Stop all sounds within a specific group.
    \param groupIndex: Index of the group whose sounds are being stopped.
    *************************************************************************/
    void Audio::StopAllSoundsInGroup(int groupIndex)
    {
        if (groupIndex < pChannelGroups.size() && pChannelGroups[groupIndex] != nullptr)
        {
            pChannelGroups[groupIndex]->setVolume(0.0f);  // Stop all sounds in the specified group
            //pChannelGroups[groupIndex]->stop();
        }
    }

    void Audio::StopAudioGroup(int groupIndex)
    {
        if (groupIndex < pChannelGroups.size() && pChannelGroups[groupIndex] != nullptr)
        {
            pChannelGroups[groupIndex]->stop();
        }
    }

    /*!***********************************************************************
    \brief
    Play all sounds within a specific group.
    \param groupIndex: Index of the group whose sounds should start playing.
    *************************************************************************/
    void Audio::PlayAllSoundsInGroup(int groupIndex)
    {
        if (groupIndex < pChannelGroups.size() && pChannelGroups[groupIndex] != nullptr)
        {
            pChannelGroups[groupIndex]->setVolume(1.0f);  // play all sounds in the specified group
        }
    }

    /*!***********************************************************************
    \brief
    Regular update function for the FMOD system.
    Must be called every frame to ensure smooth audio playback.
    *************************************************************************/
    void Audio::Update()
    {
        // Update the FMOD system regularly
        pSystem->update();
    }

    bool Audio::IsSFXPlaying(int soundIndex)
    {
        bool isPlaying = false;
        pSFXChannels[soundIndex]->isPlaying(&isPlaying);
        return isPlaying;
    }

    bool Audio::IsAnySFXPlaying()
    {
        // Loop through all channels in the list
        for (auto* channel : pSFXChannels)
        {
            // Variable to store the playing state
            bool isPlaying = false;
            // Ensure the channel is valid (not nullptr)
            if (channel)
            {
                FMOD_RESULT result = channel->isPlaying(&isPlaying);
                if (result == FMOD_OK && isPlaying)
                {
                    // If any channel is playing, return true
                    return true; 
                }
            }
        }
        // If none of the channels are playing, return false
        return false; 
    }

    /*!***********************************************************************
    \brief
    Check if a specific sound is currently playing.
    \param soundIndex: Index of the sound to check.
    \return True if the sound is playing, false otherwise.
    *************************************************************************/
    bool Audio::IsMusicPlaying(int soundIndex)
    {
        bool isPlaying = false;
        pMusicChannels[soundIndex]->isPlaying(&isPlaying);
        return isPlaying;
    }
    void Audio::RemoveMusic(int index)
    {
        if (pMusic.size() > 1)
        {
            pMusic.erase(pMusic.begin() + index);
            pMusicChannels.erase(pMusicChannels.begin() + index);
        }
        else
        {
            pMusic.clear();
            pMusicChannels.clear();
        }
    }
    void Audio::RemoveSFX(int index)
    {
        if (pSFX.size() > 1)
        {
            pSFX.erase(pSFX.begin() + index);
            pSFXChannels.erase(pSFXChannels.begin() + index);
        }
        else
        {
            pSFX.clear();
            pSFXChannels.clear();
        }
    }
}
