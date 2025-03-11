/* Start Header ************************************************************************/
/*!
\file       VideoManager.cpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Feb 5, 2025
\brief      This file handle video in mpeg format  

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"
#define PL_MPEG_IMPLEMENTATION
#include "VideoManager.h"

#include "Ukemochi-Engine/FrameController.h"
#include "Ukemochi-Engine/Graphics/Renderer.h"

namespace Ukemochi {

    void CheckGLError(const char* file, int line)
    {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::cerr << "OpenGL ERROR [" << file << ":" << line << "]: " << err << std::endl;
        }
    }
    #define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)

    bool VideoManager::IsVideoDonePlaying(const std::string& videoName)
    {
        return videos[videoName].done;
    }

    void VideoManager::RenderVideoFrame()
    {
        if (currentVideo.empty() || videos.find(currentVideo) == videos.end()) 
        {
            return; // No video selected
        }

        VideoData& video = videos[currentVideo];

        if (!video.plm) return;

#ifdef _DEBUG
        UME_ENGINE_TRACE("Rendering frame {0}/{1}", video.currentFrame, video.totalFrames);
#else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
        // Activate shader
        ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->Activate();

        // Get projection and view matrices from the camera
        glm::mat4 projection = ECS::GetInstance().GetSystem<Camera>()->getCameraProjectionMatrix();
        glm::mat4 view = ECS::GetInstance().GetSystem<Camera>()->getCameraViewMatrix();

        // Get screen dimensions (assuming full-size video)
        float screenWidth = ECS::GetInstance().GetSystem<Camera>()->viewport_size.x;
        float screenHeight = ECS::GetInstance().GetSystem<Camera>()->viewport_size.y;

        // Get video dimensions
        float videoWidth = static_cast<float>(plm_get_width(video.plm));
        float videoHeight = static_cast<float>(plm_get_height(video.plm));

        // Calculate the position for centering
        float posX = screenWidth * 0.5f;
        float posY = screenHeight * 0.5f;

        // Apply translation and scaling
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(posX, posY, 0.0f));
        model = glm::scale(model, glm::vec3(videoWidth, videoHeight, 1.0f));


        // Final MVP matrix
        glm::mat4 mvp = projection * view * model;

        // Pass it to the shader
        glUniformMatrix4fv(glGetUniformLocation(ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->ID, "mvp"),
            1, GL_FALSE, glm::value_ptr(mvp));

        // Bind VAO
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, video.textureID);
        glUniform1i(glGetUniformLocation(ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->ID, "videoTextures"), 0); 

        glUniform1i(glGetUniformLocation(ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->ID, "frameIndex"),
            video.currentFrame);

        // Draw the quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Unbind
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    void VideoManager::Audio_Callback(plm_t* plm, plm_samples_t* frame, void* user)
    {
        if (frame->count > 0) {
            Audio::GetInstance().playStereoSound(frame->interleaved, frame->count,1.f);
        }
    }

    bool VideoManager::LoadVideo(const std::string& name, const char* filepath, bool loop)
    {
        VideoData video;
        video.plm = plm_create_with_filename(filepath);
        if (!video.plm)
        {
            UME_ENGINE_ERROR("Failed to load video: {0}", filepath);
            return false;
        }

        if (!plm_probe(video.plm, 5000 * 1024))
        {
            UME_ENGINE_ERROR("No MPEG video or audio streams found in {0}", filepath);
            return false;
        }

        int width = plm_get_width(video.plm);
        int height = plm_get_height(video.plm);
        int frameRate = plm_get_framerate(video.plm);
        video.totalFrames = static_cast<int>(plm_get_duration(video.plm) * frameRate); // Total frames
        // Compute the required time per frame
        video.frameDuration = (double)(1.0 / video.totalFrames) * plm_get_duration(video.plm);
        
        video.loop = loop;

#ifdef _DEBUG
        UME_ENGINE_INFO("Total expected frames to load: {0}", video.totalFrames);

        UME_ENGINE_INFO("Opened {0} - framerate: {1}, duration: {2}, total frames: {3}",
            filepath, frameRate, plm_get_duration(video.plm), video.totalFrames);
#endif

        // Allocate storage for texture array
        video.textureID = CreateVideoTexture(name, width, height, video.totalFrames);

        // Allocate RGB buffer
        video.video_ctx = new VideoContext(width, height);
        video.video_ctx->rgb_buffer = static_cast<uint8_t*>(malloc(width * height * 3));

        // **Preload all frames into the texture array**
        glBindTexture(GL_TEXTURE_2D_ARRAY, video.textureID);
        for (int i = 0; i < video.totalFrames; ++i)
        {
            // Decode the next frame
            plm_frame_t* frame = plm_decode_video(video.plm);
            if (!frame) break;

            // Convert to RGB
            plm_frame_to_rgb(frame, video.video_ctx->rgb_buffer, width * 3);

            if (!video.video_ctx->rgb_buffer) {
                UME_ENGINE_ERROR("Error: RGB buffer is null at frame {0}", i);
            }

            // Upload to the 2D texture array at layer `i`
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, video.video_ctx->rgb_buffer);

            UME_ENGINE_INFO("Uploaded frame {0}/{1} to texture array layer {2}", i + 1, video.totalFrames, i);
        }
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        // Disable looping for playback
        plm_set_loop(video.plm, false); // Handled manually in `UpdateAndRenderVideo()

        int storedLayers = 0;
        glBindTexture(GL_TEXTURE_2D_ARRAY, video.textureID);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &storedLayers);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        UME_ENGINE_INFO("Total stored frames in OpenGL texture array: {0}", storedLayers);

        // Debug check
        if (storedLayers != video.totalFrames)
        {
            UME_ENGINE_ERROR("Mismatch! Expected {0} frames, but OpenGL stored {1}.", video.totalFrames, storedLayers);
            return false;
        }

        float videoWidth = static_cast<float>(plm_get_width(video.plm));
        float videoHeight = static_cast<float>(plm_get_height(video.plm));

        float vertices[] = {
            // Positions         // Colors      // Texture Coords
            -0.5f, -0.5f, 0.0f,  1.0f,1.0f,1.0f,   0.0f, 1.0f,  // Bottom-left (Y flipped)
             0.5f, -0.5f, 0.0f,  1.0f,1.0f,1.0f,   1.0f, 1.0f,  // Bottom-right (Y flipped)
             0.5f,  0.5f, 0.0f,  1.0f,1.0f,1.0f,   1.0f, 0.0f,  // Top-right (Y flipped)

            -0.5f, -0.5f, 0.0f,  1.0f,1.0f,1.0f,   0.0f, 1.0f,  // Bottom-left (Y flipped)
             0.5f,  0.5f, 0.0f,  1.0f,1.0f,1.0f,   1.0f, 0.0f,  // Top-right (Y flipped)
            -0.5f,  0.5f, 0.0f,  1.0f,1.0f,1.0f,   0.0f, 0.0f   // Top-left (Y flipped)
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // UV
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        //std::string filename = filepath;
        //size_t lastDot = filename.find_last_of(".");
        //if (lastDot != std::string::npos) {
        //    filename = filename.substr(0, lastDot);  // Remove extension
        //}

        //// Add your own file extension (.wav)
        //std::string newFilename = filename + ".wav";

        ECS::GetInstance().GetSystem<Audio>()->LoadSound(0, "../Assets/Video/intro-cutscene.wav", "SFX");

        videos[name] = video;  // Store the video in the map

        return true;
    }

    GLuint VideoManager::CreateVideoTexture(const std::string& videoName, int width, int height, int num_frames)
    {
        stbi_set_flip_vertically_on_load(true);
        GLuint textureID; 
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

        if (!textureID)
        {
            UME_ENGINE_ERROR("Failed to generate video texture!");
            return 0;
        }

        glTexStorage3D(GL_TEXTURE_2D_ARRAY,1,GL_RGB8 ,width,height,num_frames);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        UME_ENGINE_TRACE("Created Video Texture - ID: {0}, Width: {1}, Height: {2}, Frames: {3}", textureID, width, height, num_frames);

        // Store texture ID in the map
        videos[videoName].textureID = textureID;

        return textureID;
      
    }

    void VideoManager::UpdateAndRenderVideo()
    {
        if (currentVideo.empty() || videos.find(currentVideo) == videos.end()) 
        {
            UME_ENGINE_ERROR("No video selected or video not found!");
            return;
        }

        VideoData& video = videos[currentVideo];

        if (!video.plm) 
        {
            UME_ENGINE_ERROR("Current video object is null!");
            return;
        }

        float deltaTime = g_FrameRateController.GetDeltaTime();
#ifdef _DEBUG
        UME_ENGINE_TRACE("Decoding Video - DeltaTime: {0}", deltaTime);
#endif
        Audio::GetInstance().Update(); // Update audio

        // Get the elapsed time since the last frame
        video.elapsedTime += deltaTime;
        //static double audioDuration = (double)(1.0 / video.totalFrames) * 14;

        //if (video.currentFrame >= 400)
        //{
        //    if (ECS::GetInstance().GetSystem<Audio>()->IsSFXPlaying(0))
        //    {
        //        ECS::GetInstance().GetSystem<Audio>()->StopSFX(0);
        //    }
        //}

        if (ECS::GetInstance().GetSystem<Audio>()->CheckSFX())
        {
            if (!ECS::GetInstance().GetSystem<Audio>()->IsSFXPlaying(0))
            {
                ECS::GetInstance().GetSystem<Audio>()->PlaySound(0, "SFX");
            }
        }


        // Only update the video frame if enough time has passed
        if (video.currentFrame < video.totalFrames - 1)
        {
            //if (video.elapsedTime >= audioDuration)
            //{
            //    if (!ECS::GetInstance().GetSystem<Audio>()->IsSFXPlaying(0))
            //    {
            //        ECS::GetInstance().GetSystem<Audio>()->PlaySound(0, "SFX");
            //    }

            //}
            if (video.elapsedTime >= video.frameDuration)
            {
                // Reset elapsed time to stay in sync with video playback
                video.elapsedTime = 0.f;

                // Move to the next frame
                video.currentFrame = (video.currentFrame + 1) % video.totalFrames;
            }
        }
        else
        {
            if (video.loop)
            {
                // Restart the video if it's a looping video
                video.currentFrame = 0;
                video.elapsedTime = 0.f;
            }
            else
            {
                Free(); // Free resources if not looping 
                return;
            }
        }

        // Render the current video frame
        RenderVideoFrame();
    }

    void VideoManager::SetCurrentVideo(const std::string& name)
    {
        if (videos.find(name) == videos.end()) 
        {
            UME_ENGINE_ERROR("Video {0} not found!", name);
            return;
        }
        currentVideo = name;
    }


    void VideoManager::Update()
    {
        UpdateAndRenderVideo();  
    }
    void VideoManager::SkipVideo()
    {
        UME_ENGINE_TRACE("Skipping video...");

        // Stop any playing audio
        ECS::GetInstance().GetSystem<Audio>()->DeleteSound(0, "SFX");

        // Free video resources
        Free();
    }

    void VideoManager::Free()
    {
        VideoData& video = videos[currentVideo];
        ECS::GetInstance().GetSystem<Audio>()->DeleteSound(0, "SFX");
        // Set the camera initial position
        //ECS::GetInstance().GetSystem<Camera>()->position = { -1920, 0 };

        // Free PLM object
        if (video.plm)
        {
            plm_destroy(video.plm);
            video.plm = nullptr;
        }

        // Delete OpenGL texture
        if (video.textureID)
        {
            glDeleteTextures(1, &video.textureID);
            video.textureID = 0;
        }
        // Remove video entry from map
        //videos.erase(currentVideo);
        //currentVideo.clear(); // No active video
        
        if (video.video_ctx)
        {
            if (video.video_ctx->rgb_buffer)
            {
                free(video.video_ctx->rgb_buffer); // Ensure correct allocation type
            }
            delete video.video_ctx;
        }

        video.done = true;
        //delete video_ctx;
        //delete rb;
    }
}
