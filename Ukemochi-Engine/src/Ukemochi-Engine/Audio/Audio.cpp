/* Start Header ************************************************************************/
/*!
\file       Audio.cpp
\author     Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu
\date       Dec 1, 2024
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
#include "../Input/Input.h"

namespace Ukemochi
{

    std::vector<float> Audio::pcm32Data;  // Use float instead of int16_t
    static size_t readPosition = 0;

    FMOD_RESULT F_CALLBACK Audio::pcmReadCallback(FMOD_SOUND* sound, void* data, unsigned int datalen)
    {
        std::cout << "PCM Read Callback Triggered. Data Size: " << datalen << std::endl;

        if (pcm32Data.empty()) {
            return FMOD_ERR_INVALID_PARAM;
        }

        size_t dataSizeNeeded = datalen / sizeof(float);
        if (pcm32Data.size() < dataSizeNeeded) {
            pcm32Data.resize(dataSizeNeeded);
        }

        size_t remainingData = pcm32Data.size() - readPosition;

        if (remainingData < dataSizeNeeded) {
            readPosition = 0;
            remainingData = pcm32Data.size();
        }

        memcpy(data, pcm32Data.data() + readPosition, datalen);
        readPosition += dataSizeNeeded;

        return FMOD_OK;
    }

    void Audio::playStereoSound(float* interleavedSamples, int sampleCount)
    {
        if (!pSystem) {
            std::cerr << "FMOD system is not initialized!" << std::endl;
            return;
        }

        if (!interleavedSamples) {
            std::cerr << "Error: interleavedSamples is nullptr!" << std::endl;
            return;
        }

        if (sampleCount <= 0) {
            std::cerr << "Error: sampleCount is invalid (" << sampleCount << ")!" << std::endl;
            return;
        }

        // Compute total samples (stereo has 2 channels)
        size_t totalSamples = static_cast<size_t>(sampleCount) * 2;


        // Ensure pcm32Data is large enough
        pcm32Data.resize(totalSamples, 0.0f);

        // Low-pass filter parameters
        float previousSampleL = 0.0f;
        float previousSampleR = 0.0f;
        float alpha = 0.01f;

        // Apply low-pass filter while copying samples
        for (size_t i = 0; i < totalSamples; i += 2) {
            // Check if index is valid before accessing
            if (i >= totalSamples || (i + 1) >= totalSamples) {
                return;
            }

            // Verify interleavedSamples contains valid data
            if (!std::isfinite(interleavedSamples[i]) || !std::isfinite(interleavedSamples[i + 1])) {
                return;
            }

            // Left channel
            pcm32Data[i] = previousSampleL + alpha * (interleavedSamples[i] - previousSampleL);
            previousSampleL = pcm32Data[i];

            // Right channel
            pcm32Data[i + 1] = previousSampleR + alpha * (interleavedSamples[i + 1] - previousSampleR);
            previousSampleR = pcm32Data[i + 1];
        }

        // Ensure PCM data is within range [-1.0f, 1.0f]
        for (auto& sample : pcm32Data) {
            sample = std::clamp(sample, -1.0f, 1.0f);
        }

        // If a sound is already playing, release it before creating a new one
        if (pvideosound) {
            pvideosound->release();
            pvideosound = nullptr;
        }

        // Configure FMOD sound settings
        FMOD_CREATESOUNDEXINFO exinfo = {};
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        exinfo.length = totalSamples * sizeof(float);
        exinfo.format = FMOD_SOUND_FORMAT_PCMFLOAT;
        exinfo.numchannels = 2;
        exinfo.defaultfrequency = 48000; // Adjust based on your source audio
        exinfo.pcmreadcallback = pcmReadCallback; // Custom PCM read callback

        // Create a user-defined FMOD sound
        FMOD_RESULT result = pSystem->createSound(
            nullptr,  // Data is provided via callback
            FMOD_OPENUSER | FMOD_LOOP_OFF | FMOD_3D_HEADRELATIVE,
            &exinfo,
            &pvideosound
        );

        if (result != FMOD_OK) {
            std::cerr << "FMOD Error (createSound): " << result << std::endl;
            return;
        }

        // Play the sound
        FMOD::Channel* pChannel = nullptr;
        result = pSystem->playSound(pvideosound, nullptr, false, &pChannel);
        if (result != FMOD_OK) {
            std::cerr << "FMOD Error (playSound): " << result << std::endl;
        }
        else {
            std::cout << "Playing audio!" << std::endl;
        }
    }

    /*!***********************************************************************
    \brief
     Constructor for the Audio class.
     This is where system initialization and resource allocation happen.
    *************************************************************************/
    Audio::Audio()
        : pSystem(nullptr), numOfMusic(0), numOfSFX(0),pLockedData(nullptr), pLockedDataLength(0)
    {
        pvideosound = nullptr;
        pvideoChannel = nullptr;
        FMOD_RESULT result;

        // Create FMOD system
        result = FMOD::System_Create(&pSystem);
        if (result != FMOD_OK)
        {
            std::cerr << "FMOD system creation failed: " << result << std::endl;
            return;
        }

        // Initialize the FMOD system with 32 channels
        result = pSystem->init(512, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            std::cerr << "FMOD system initialization failed: " << result << std::endl;
            return;
        }
        pSystem->setDSPBufferSize(2048, 4);
        CreateGroup();
    }

    /*!***********************************************************************
    \brief
     Destructor for the Audio class.
     This is where system cleanup and resource deallocation happen.
    *************************************************************************/
    Audio::~Audio()
    {
        delete[] pLockedData;
        // Release all sounds
        pvideosound->release();
        pvideoChannel = nullptr;

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
        Delete a sound from the loaded audio.
    \param index
        Index of the sound file to be deleted.
    \param filePath
        The file path to the sound file to be deleted.
    \param type
        Type of sound, either "SFX" or "Music".
    \return
        True if the sound was successfully deleted, false otherwise.
    *************************************************************************/
    bool Audio::DeleteSound(int index, std::string type)
    {
        if (type == "SFX")
        {
            // Validate index
            if (index < 0 || index >= pSFX.size())
            {
                std::cerr << "Invalid index for SFX: " << index << std::endl;
                return false;
            }

            // Release the sound
            FMOD_RESULT result = pSFX[index]->release();
            if (result != FMOD_OK)
            {
                std::cerr << "Failed to release SFX sound: " << result << std::endl;
                return false;
            }

            // Remove sound and channel from the vectors
            pSFX.erase(pSFX.begin() + index);
            pSFXChannels.erase(pSFXChannels.begin() + index);
            --numOfSFX;

            return true;
        }
        else if (type == "Music")
        {
            // Validate index
            if (index < 0 || index >= pMusic.size())
            {
                std::cerr << "Invalid index for Music: " << index << std::endl;
                return false;
            }

            // Release the sound
            FMOD_RESULT result = pMusic[index]->release();
            if (result != FMOD_OK)
            {
                std::cerr << "Failed to release Music sound: " << result << std::endl;
                return false;
            }

            // Remove sound and channel from the vectors
            pMusic.erase(pMusic.begin() + index);
            pMusicChannels.erase(pMusicChannels.begin() + index);
            --numOfMusic;

            return true;
        }

        // If the type is neither SFX nor Music, return false
        std::cerr << "Invalid type specified: " << type << std::endl;
        return false;
    }

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
    bool Audio::LoadSound(int index, const char *filePath, std::string type)
    {
        FMOD::Sound *sound = nullptr;
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
            if (pSFX.empty() || index == pSFX.size())
            {
                pSFX.push_back(sound);
                pSFXChannels.push_back(nullptr); // Add a corresponding channel for each sound
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
                pMusicChannels.push_back(nullptr); // Add a corresponding channel for each sound
                ++numOfMusic;
            }
            else
            {
                pMusic[index] = sound;
            }
        }
        return true;
    }

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
    void Audio::PlaySound(int soundIndex, std::string type)
    {
        if (soundIndex < pSFX.size() || soundIndex < pMusic.size())

        {
            FMOD_RESULT result;
            FMOD::Channel *channel = nullptr;

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

                // std::cout << "Sound " << soundIndex << " is playing in group " << soundIndex << std::endl;
            }
            else if (type == "Music")
            {
                result = pMusic[soundIndex]->setMode(FMOD_LOOP_NORMAL);
                if (result != FMOD_OK)
                {
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
                // std::cout << "Sound " << soundIndex << " is playing in group " << soundIndex << std::endl;
            }
        }
        else
        {
            std::cerr << "Invalid sound or group index!" << std::endl;
        }
    }

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
                    pSFXChannels[soundIndex]->stop(); // Stop the sound if it's playing
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
                    pMusicChannels[soundIndex]->stop(); // Stop the sound if it's playing
                }
            }
        }
    }

    /*!***********************************************************************
    \brief
        Stop all sounds currently playing (both SFX and Music).
    \param None.
    \return
        None.
    \note
        Stops all sounds in the SFX and Music channels.
    *************************************************************************/
    void Audio::StopAllSound()
    {
        for (auto *channel : pSFXChannels)
        {
            channel->stop();
        }
        for (auto *channel : pMusicChannels)
        {
            channel->stop();
        }
    }

    /*!***********************************************************************
    \brief
        Play the background music if it is not already playing.
    \param None.
    \return
        None.
    \note
        Checks if the background music is already playing. If not, it plays the music from the first entry in the music list.
    *************************************************************************/
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
    Stop a sound effect based on its index.
\param sfxIndex
    The index of the sound effect to be stopped.
\return
    None.
\note
    Stops the sound effect if it is currently playing.
*************************************************************************/
    void Audio::StopSFX(int sfxIndex)
    {
        if (sfxIndex < 0 || sfxIndex >= numOfSFX)
        {
            std::cerr << "Invalid SFX index: " << sfxIndex << std::endl;
            return;
        }

        bool isPlaying = false;
        if (pSFXChannels[sfxIndex])
        {
            pSFXChannels[sfxIndex]->isPlaying(&isPlaying);

            if (isPlaying)
            {
                pSFXChannels[sfxIndex]->stop();  // Stop the sound effect if it's playing
                std::cout << "Stopped SFX at index " << sfxIndex << std::endl;
            }
        }
        else
        {
            std::cerr << "SFX channel not initialized for index " << sfxIndex << std::endl;
        }
    }

    /*!***********************************************************************
    \brief
        Stop all sound effects currently playing.
    \param None.
    \return
        None.
    \note
        Stops all active sound effects.
    *************************************************************************/
    void Audio::StopAllSFX()
    {
        for (size_t i = 0; i < pSFXChannels.size(); ++i)
        {
            StopSFX(i);  // Stop each individual SFX
        }
    }



    /*!***********************************************************************
    \brief
    Toggle a sound in a group (play or pause).
    \param soundIndex: Index of the sound to toggle.
    \param groupIndex: Index of the group where the sound resides.
    *************************************************************************/
    // void Audio::ToggleSoundInGroup(int soundIndex, int groupIndex)
    //{
    //     if (soundIndex < numOfAudios && groupIndex < pChannelGroups.size())
    //     {
    //         bool isPlaying = false;

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
        Set the volume of a specific sound (SFX or Music).
    \param soundIndex: The index of the sound to adjust the volume.
    \param volume: The volume level to set (range: 0.0f to 1.0f).
    \param type: The type of sound ("SFX" or "Music").
    \return
        None.
    \note
        Adjusts the volume of a specific sound based on the given index and type.
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
    Regular update function for the FMOD system.
    Must be called every frame to ensure smooth audio playback.
    *************************************************************************/
    void Audio::Update()
    {
        // Update the FMOD system regularly
        pSystem->update();

        // Handle muting/unmuting music and SFX
        static bool musicKeyPressed = false;
        static bool sfxKeyPressed = false;

        // Check if the 'K' key is pressed for muting/unmuting music
        if (Input::IsKeyPressed(UME_KEY_K))
        {
            if (!musicKeyPressed)
            {
                MuteMusic(); // Call the method to toggle mute for music
                musicKeyPressed = true;
            }
        }
        else
        {
            musicKeyPressed = false;
        }
    }

    /*!***********************************************************************
    \brief
        Check if a specific SFX is currently playing.
    \param soundIndex: The index of the SFX to check.
    \return
        True if the SFX is playing, false otherwise.
    \note
        This function checks the playing state of a specific SFX channel by index.
    *************************************************************************/
    bool Audio::IsSFXPlaying(int soundIndex)
    {
        bool isPlaying = false;
        pSFXChannels[soundIndex]->isPlaying(&isPlaying);
        return isPlaying;
    }

    /*!***********************************************************************
    \brief
        Check if any SFX is currently playing.
    \return
        True if any SFX is playing, false otherwise.
    \note
        This function checks all SFX channels and returns true if any are playing.
    *************************************************************************/
    bool Audio::IsAnySFXPlaying()
    {
        // Loop through all channels in the list
        for (auto *channel : pSFXChannels)
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
        Check if a specific music track is currently playing.
    \param soundIndex: The index of the music track to check.
    \return
        True if the music track is playing, false otherwise.
    \note
        This function checks the playing state of a specific music channel by index.
    *************************************************************************/
    bool Audio::IsMusicPlaying(int soundIndex)
    {
        bool isPlaying = false;
        pMusicChannels[soundIndex]->isPlaying(&isPlaying);
        return isPlaying;
    }

    /*!***********************************************************************
    \brief
        Remove a music track from the list by index.
    \param index: The index of the music track to remove.
    \note
        This function removes a music track and its corresponding channel from the list.
        If the list contains only one music track, both the track and channel are cleared.
    *************************************************************************/
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

    /*!***********************************************************************
    \brief
       Remove a sound effect (SFX) from the list by index.
    \param index: The index of the SFX to remove.
    \note
       This function removes an SFX and its corresponding channel from the list.
       If the list contains only one SFX, both the SFX and channel are cleared.
    *************************************************************************/
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

    void Audio::MuteMusic()
    {
        isMusicMuted = !isMusicMuted; // Toggle mute state

        // Update the volume of all music channels
        for (auto* channel : pMusicChannels)
        {
            if (channel)
            {
                channel->setVolume(isMusicMuted ? 0.0f : musicVolume);
            }
        }

        std::cout << (isMusicMuted ? "Music muted." : "Music unmuted.") << std::endl;
    }

    void Audio::MuteSFX()
    {
        isSFXMuted = !isSFXMuted; // Toggle mute state

        // Update the volume of all SFX channels
        for (auto* channel : pSFXChannels)
        {
            if (channel)
            {
                channel->setVolume(isSFXMuted ? 0.0f : sfxVolume);
            }
        }

        std::cout << (isSFXMuted ? "SFX muted." : "SFX unmuted.") << std::endl;
    }

    // NOT IN USED

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

        // NOT IN USED
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
            pChannelGroups[groupIndex]->setVolume(volume); // Set the volume for the specific group
        }
    }

    /*!***********************************************************************
    \brief
    Play a sound in a specified group.
    \param soundIndex: Index of the sound to play.
    \param groupIndex: Index of the group in which the sound should be played.
    *************************************************************************/
    // void Audio::PlaySoundInGroup(int soundIndex, int groupIndex)
    //{
    //     if (soundIndex < numOfAudios && groupIndex < pChannelGroups.size())
    //     {
    //         FMOD_RESULT result;
    //         FMOD::Channel* channel = nullptr;

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

    // NOT IN USED
    /*!***********************************************************************
    \brief
    Stop all sounds within a specific group.
    \param groupIndex: Index of the group whose sounds are being stopped.
    *************************************************************************/
    void Audio::StopAllSoundsInGroup(int groupIndex)
    {
        if (groupIndex < pChannelGroups.size() && pChannelGroups[groupIndex] != nullptr)
        {
            pChannelGroups[groupIndex]->setVolume(0.0f); // Stop all sounds in the specified group
            // pChannelGroups[groupIndex]->stop();
        }
    }
    // NOT IN USED
    void Audio::StopAudioGroup(int groupIndex)
    {
        if (groupIndex < pChannelGroups.size() && pChannelGroups[groupIndex] != nullptr)
        {
            pChannelGroups[groupIndex]->stop();
        }
    }

    // NOT IN USED
    /*!***********************************************************************
    \brief
    Play all sounds within a specific group.
    \param groupIndex: Index of the group whose sounds should start playing.
    *************************************************************************/
        void Audio::PlayAllSoundsInGroup(int groupIndex)
        {
            if (groupIndex < pChannelGroups.size() && pChannelGroups[groupIndex] != nullptr)
            {
                pChannelGroups[groupIndex]->setVolume(1.0f); // play all sounds in the specified group
            }
        }
}

