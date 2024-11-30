/* Start Header ************************************************************************/
/*!
\file       Audio.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu
\date       Oct 4, 2024
\brief      This file contains the declaration of the Audio system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef AUDIO_H
#define AUDIO_H

#include <fmod.hpp>
#include <iostream>
#include <vector>
#include "../ECS/Systems.h"

namespace Ukemochi
{
    // Audio class, responsible for managing all audio-related functionality
    class Audio : public System
    {
    public:

        /*!***********************************************************************
        \brief
         Making Audio class object as a Singleton object
        *************************************************************************/
        static Audio& GetInstance()
        {
            static std::unique_ptr<Audio> instance(new Audio());
            return *instance;
        }

        /*!***********************************************************************
        \brief
         Constructor for the Audio class.
         This is where system initialization and resource allocation happen.
        *************************************************************************/
        Audio();

        /*!***********************************************************************
        \brief
         Destructor for the Audio class.
         This is where system cleanup and resource deallocation happen.
        *************************************************************************/
        ~Audio();

        /*!***********************************************************************
        \brief
         Create a new ChannelGroup for managing sound channels.
         ChannelGroups help manage a group of channels as one unit.
        *************************************************************************/
        void CreateGroup();

        /*!***********************************************************************
        \brief
         Load a sound from a file.
         \param filePath: The file path to the sound file to be loaded.
         \return True if the sound was successfully loaded, false otherwise.
        *************************************************************************/
        bool LoadSound(int index,const char* filePath,std::string type);

        void PlaySound(int soundIndex, std::string type);

        /*!***********************************************************************
        \brief
         Play a sound in a specified group.
         \param soundIndex: Index of the sound to play.
         \param groupIndex: Index of the group in which the sound should be played.
        *************************************************************************/
        //void PlaySoundInGroup(int soundIndex, int groupIndex);

        /*!***********************************************************************
        \brief
         Stop playing a specific sound.
         \param soundIndex: Index of the sound to stop.
        *************************************************************************/
        void StopSound(int soundIndex, std::string type);

        void StopAllSound();

        /*!***********************************************************************
        \brief
         Toggle a sound in a group (play or pause).
         \param soundIndex: Index of the sound to toggle.
         \param groupIndex: Index of the group where the sound resides.
        *************************************************************************/
        //void ToggleSoundInGroup(int soundIndex, int groupIndex);

        /*!***********************************************************************
        \brief
         Set the volume for a specific sound.
         \param soundIndex: Index of the sound whose volume is being set.
         \param volume: The volume level (0.0 to 1.0).
        *************************************************************************/
        void SetAudioVolume(int soundIndex, float volume, std::string type);

        /*!***********************************************************************
        \brief
         Set the volume for an entire group of sounds.
         \param groupIndex: Index of the group whose volume is being set.
         \param volume: The volume level (0.0 to 1.0).
        *************************************************************************/
        void SetGroupVolume(int groupIndex, float volume);

        /*!***********************************************************************
        \brief
         Stop all sounds within a specific group.
         \param groupIndex: Index of the group whose sounds are being stopped.
        *************************************************************************/
        void StopAllSoundsInGroup(int groupIndex);

        void StopAudioGroup(int groupIndex);

        /*!***********************************************************************
        \brief
         Play all sounds within a specific group.
         \param groupIndex: Index of the group whose sounds should start playing.
        *************************************************************************/
        void PlayAllSoundsInGroup(int groupIndex);

        /*!***********************************************************************
        \brief
         Regular update function for the FMOD system.
         Must be called every frame to ensure smooth audio playback.
        *************************************************************************/
        void Update();

        /*!***********************************************************************
        \brief
         Check if a specific sound is currently playing.
         \param soundIndex: Index of the sound to check.
         \return True if the sound is playing, false otherwise.
        *************************************************************************/
        bool IsSFXPlaying(int soundIndex);

        bool IsAnySFXPlaying();

        bool IsMusicPlaying(int soundIndex);

        void RemoveMusic(int index);
        void RemoveSFX(int index);

        std::vector<FMOD::Sound*> pSFX;  // A list of loaded sounds
        std::vector<FMOD::Channel*> pSFXChannels;  // A list of channels playing individual sounds

        std::vector<FMOD::Sound*> pMusic;  // A list of loaded sounds
        std::vector<FMOD::Channel*> pMusicChannels;  // A list of channels playing individual sounds

        FMOD::System* pSystem;  // Pointer to the FMOD system, which manages all sound operations

    private:

        std::vector<FMOD::ChannelGroup*> pChannelGroups;  // A list of channel groups for managing groups of sounds
        int numOfSFX;  // A counter to track the number of loaded sounds
        int numOfMusic;
    };
}

#endif // AUDIO_H
