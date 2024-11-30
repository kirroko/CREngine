/* Start Header ************************************************************************/
/*!
\file       Audio.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu
\date       Dec 1, 2024
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
        Constructor for the Audio class.
        This is where system initialization and resource allocation happen.
        *************************************************************************/
        static Audio &GetInstance()
        {
            static std::unique_ptr<Audio> instance(new Audio());
            return *instance;
        }

        /*!***********************************************************************
        \brief
         Destructor for the Audio class.
         This is where system cleanup and resource deallocation happen.
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
        \param index
            Index to the sound file to be loaded.
        \param filePath
            The file path to the sound file to be loaded.
        \param type
            Type of sound, either "SFX" or "Music".
        \return
            True if the sound was successfully loaded, false otherwise.
        *************************************************************************/
        bool LoadSound(int index, const char *filePath, std::string type);

        /*!***********************************************************************
        \brief
            Play a sound based on its index and type (SFX or Music).
        \param soundIndex
            The index of the sound to be played.
        \param type
            The type of sound, either "SFX" or "Music".
        \return
            None.
        \note
            For Music, the sound is set to loop indefinitely.
            For SFX, the sound is played once with a specified volume.
        *************************************************************************/
        void PlaySound(int soundIndex, std::string type);

        /*!***********************************************************************
        \brief
         Play a sound in a specified group.
         \param soundIndex: Index of the sound to play.
         \param groupIndex: Index of the group in which the sound should be played.
        *************************************************************************/
        // void PlaySoundInGroup(int soundIndex, int groupIndex);

        /*!***********************************************************************
        \brief
        Stop a sound based on its index and type (SFX or Music).
        \param soundIndex
        The index of the sound to be stopped.
        \param type
        The type of sound, either "SFX" or "Music".
        \return
        None.
        \note
        Stops the sound if it's currently playing. The function checks if the sound is playing before stopping it.
        *************************************************************************/
        void StopSound(int soundIndex, std::string type);

        /*!***********************************************************************
        \brief
            Stop all sounds currently playing (both SFX and Music).
        \param None.
        \return
            None.
        \note
            Stops all sounds in the SFX and Music channels.
        *************************************************************************/
        void StopAllSound();

        /*!***********************************************************************
        \brief
            Play the background music if it is not already playing.
        \param None.
        \return
            None.
        \note
            Checks if the background music is already playing. If not, it plays the music from the first entry in the music list.
        *************************************************************************/
        void PlayGameBGM();

        /*!***********************************************************************
        \brief
         Toggle a sound in a group (play or pause).
         \param soundIndex: Index of the sound to toggle.
         \param groupIndex: Index of the group where the sound resides.
        *************************************************************************/
        // void ToggleSoundInGroup(int soundIndex, int groupIndex);

        /*!***********************************************************************
        \brief
            Set the volume of a specific sound (SFX or Music).
        \param soundIndex: The index of the sound to adjust the volume.
        \param volume: The volume level to set (range: 0.0f to 1.0f).
        \param type: The type of sound ("SFX" or "Music").
        \return
            None.
        \note
            Adjusts the volume of a specific sound based on the given index and type.
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
            Check if a specific SFX is currently playing.
        \param soundIndex: The index of the SFX to check.
        \return
            True if the SFX is playing, false otherwise.
        \note
            This function checks the playing state of a specific SFX channel by index.

        *************************************************************************/
        bool IsSFXPlaying(int soundIndex);

        /*!***********************************************************************
        \brief
            Check if any SFX is currently playing.
        \return
            True if any SFX is playing, false otherwise.
        \note
            This function checks all SFX channels and returns true if any are playing.
        *************************************************************************/
        bool IsAnySFXPlaying();

        /*!***********************************************************************
        \brief
            Check if a specific music track is currently playing.
        \param soundIndex: The index of the music track to check.
        \return
            True if the music track is playing, false otherwise.
        \note
            This function checks the playing state of a specific music channel by index.
        *************************************************************************/

        bool IsMusicPlaying(int soundIndex);

        /*!***********************************************************************
        \brief
            Remove a music track from the list by index.
        \param index: The index of the music track to remove.
        \note
            This function removes a music track and its corresponding channel from the list.
            If the list contains only one music track, both the track and channel are cleared.
        *************************************************************************/

        void RemoveMusic(int index);

        /*!***********************************************************************
        \brief
           Remove a sound effect (SFX) from the list by index.
        \param index: The index of the SFX to remove.
        \note
           This function removes an SFX and its corresponding channel from the list.
           If the list contains only one SFX, both the SFX and channel are cleared.
        *************************************************************************/

        void RemoveSFX(int index);

        std::vector<FMOD::Sound *> pSFX;           // A list of loaded sounds
        std::vector<FMOD::Channel *> pSFXChannels; // A list of channels playing individual sounds

        std::vector<FMOD::Sound *> pMusic;           // A list of loaded sounds
        std::vector<FMOD::Channel *> pMusicChannels; // A list of channels playing individual sounds

        FMOD::System *pSystem; // Pointer to the FMOD system, which manages all sound operations

    private:
        std::vector<FMOD::ChannelGroup *> pChannelGroups; // A list of channel groups for managing groups of sounds
        int numOfSFX;                                     // A counter to track the number of loaded sounds
        int numOfMusic;
    };
}

#endif // AUDIO_H
