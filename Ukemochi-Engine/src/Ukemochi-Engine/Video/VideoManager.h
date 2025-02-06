/* Start Header ************************************************************************/
/*!
\file       VideoManager.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Feb 5, 2025
\brief      This file handle video in mpeg format 

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once
#include "../ECS/ECS.h"
#include "pl_mpeg.h"
#include "Ukemochi-Engine/Graphics/shaderClass.h"

namespace Ukemochi {
    class VideoManager : public System
    {
        struct RingBuffer
        {
            float *buffer;
            size_t capacity; // in samples
            size_t write_index;
            size_t read_index;

            RingBuffer() : buffer(static_cast<float*>(malloc(4096))), capacity(4096), write_index(0), read_index(0)  {}
        } ;

        struct VideoContext
        {
            unsigned int width;
            unsigned int height;
            GLuint texture_crop_size;
            uint8_t *rgb_buffer;

            VideoContext(const int w, const int h) : width(w), height(h), texture_crop_size(0), rgb_buffer(nullptr) {}
        };

        RingBuffer* rb = {};
        VideoContext* video_ctx = {};
        plm_t* plm = nullptr;
        FMOD::Sound *fmod_sound = nullptr; // video sound

        std::shared_ptr<Shader> video_shader_program;
    public:
        GLuint videoTextureID = 0;
        int currentFrame = 0;
    private:
        bool isPlaying = true;

        static void Video_Callback(plm_t *plm, plm_frame_t *frame, void *user);

        static void Audio_Callback(plm_t *plm, plm_samples_t *frame, void *user);
        
        /**
         * @brief Create an OpenGL texture to store the video
         * @param width width of texture
         * @param height height of texture
         * @param num_frames
         * @return texture ID
         */
        GLuint CreateVideoTexture(int width, int height, int num_frames);
        
        /**
         * @brief decode and display the video
         * @param video the video to display
         */
        void UpdateAndRenderVideo(plm_t* video);

        FMOD_RESULT F_CALLBACK pcmReadCallback(FMOD_SOUND* sound, void* data, unsigned int datalen);
    public:
        /**
         * @brief Load the MPEG file
         * @param filepath filepath to the video
         * @return a pointer to MPEGStream
         */
        bool LoadVideo(const char* filepath);
        
        /**
         * @brief Init the video manager by creating a textureID
         * @param width width of the video texture
         * @param height height of the video texture
         */
        void Init(int width, int height);
        
        /**
         * @brief Update video player system
         */
        void Update();

        /**
         * @brief Release resources
         */
        void Free() const;
    };
}
