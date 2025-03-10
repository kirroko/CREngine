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
#include "Ukemochi-Engine/Graphics/Texture.h"
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

        struct VideoData {
            plm_t* plm = nullptr;
            GLuint textureID = 0;
            int currentFrame = 0;
            int totalFrames = 0;
            double frameDuration = 0.0;
            double elapsedTime = 0.0f;
            bool done = false;
            VideoContext* video_ctx = {};
            bool loop = false;
        };

        RingBuffer* rb = {};
        

        std::shared_ptr<Shader> video_shader_program;

    public:

        std::unordered_map<std::string, VideoData> videos; 
        std::string currentVideo; // Name of the currently playing video

        bool IsVideoDonePlaying(const std::string& videoName);

    private:
        
        double lastFrameTime = 0.0f;
        GLuint VAO{}, VBO{};

        void RenderVideoFrame();

        static void Video_Callback(plm_t *plm, plm_frame_t *frame, void *user);

        static void Audio_Callback(plm_t *plm, plm_samples_t *frame, void *user);
        
        /**
         * @brief Create an OpenGL texture to store the video
         * @param width width of texture
         * @param height height of texture
         * @param num_frames
         * @return texture ID
         */
        GLuint CreateVideoTexture(const std::string& videoName, int width, int height, int num_frames);
        
        /**
         * @brief decode and display the video
         * @param video the video to display
         */
        void UpdateAndRenderVideo();

        //FMOD_RESULT F_CALLBACK pcmReadCallback(FMOD_SOUND* sound, void* data, unsigned int datalen);
    public:
        /**
         * @brief Set current video to play
         * @param Name of the video
         */
        void SetCurrentVideo(const std::string& name);

        /**
         * @brief Load the MPEG file
         * @param filepath filepath to the video
         * @return a pointer to MPEGStream
         */
        bool LoadVideo(const std::string& name, const char* filepath, bool loop);
        
        /**
         * @brief Update video player system
         */
        void Update();

        /**
         * @brief Skips cut scene
         */
        void SkipVideo();

        /**
         * @brief Release resources
         */
        void Free();
    };
}
