/* Start Header ************************************************************************/
/*!
\file       VideoManager.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\co-author  TAN SHUN ZHI, Tomy, t.shunzhitomy, 2301341, t.shunzhitomy\@digipen.edu
\co-author  TAN SI HAN, t.sihan, 2301264, t.sihan\@digipen.edu
\date       Mar 12, 2025
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
        // Job system for video decoding
        struct VideoDecodeJobParams
        {
            char* filename;
            uint8_t* rgb_buffer;
            double frameDuration;
            int width;
            unsigned int frameIndex;
        };
        
        /*!***********************************************************************
        \brief
         Represents the context for a video frame.

        \details
         This struct holds width, height, and buffer data used for video decoding.
        *************************************************************************/
        struct VideoContext
        {
            unsigned int width;
            unsigned int height;
            GLuint texture_crop_size;
            std::unique_ptr<uint8_t[]> rgb_buffer;

            VideoContext(const int w, const int h) : width(w), height(h), texture_crop_size(0), rgb_buffer(std::make_unique<uint8_t[]>(w* h * 3)) {}
        };

        /*!***********************************************************************
        \brief
         Stores metadata and state information for a video.

        \details
         This struct maintains video decoding state, texture ID, frame tracking,
         playback timing, and looping information.
        *************************************************************************/
        struct VideoData {
            plm_t* plm = nullptr;
            GLuint textureID = 0;
            int currentFrame = 0;
            int totalFrames = 0;
            double frameDuration = 0.0;
            double elapsedTime = 0.0f;
            bool done = false;
            std::unique_ptr<VideoContext> video_ctx;
            bool loop = false;
        };

        std::shared_ptr<Shader> video_shader_program; // Shader program used for rendering video

    public:

        std::unordered_map<std::string, VideoData> videos; // Map of video names to their data
        std::string currentVideo; // Name of the currently playing video

        /*!***********************************************************************
        \brief
         Checks if a video has finished playing.

        \param videoName The name of the video to check.
        \return True if the video has finished playing, false otherwise.
        *************************************************************************/
        bool IsVideoDonePlaying(const std::string& videoName);

    private:
        
        double lastFrameTime = 0.0f; // Timestamp of the last frame update
        GLuint VAO{}, VBO{}; // OpenGL Vertex Array Object and Vertex Buffer Object

        /*!***********************************************************************
        \brief
         Renders the current frame of the active video.

        \details
         This function retrieves the currently playing video, applies the appropriate
         transformations, and renders the frame using OpenGL.
        *************************************************************************/
        void RenderVideoFrame();
        
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

        void CleanupAllVideos();
    };
}
