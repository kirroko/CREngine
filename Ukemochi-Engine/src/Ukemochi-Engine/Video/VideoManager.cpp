/* Start Header ************************************************************************/
/*!
\file       VideoManager.cpp
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
#include "PreCompile.h"
#define PL_MPEG_IMPLEMENTATION
#include "VideoManager.h"

#include "Ukemochi-Engine/FrameController.h"
#include "Ukemochi-Engine/Graphics/Renderer.h"

namespace Ukemochi {

    /*!***********************************************************************
    \brief
     Checks if a video has finished playing.

    \param videoName The name of the video to check.
    \return True if the video has finished playing, false otherwise.
    *************************************************************************/
    bool VideoManager::IsVideoDonePlaying(const std::string& videoName)
    {
        return videos[videoName].done;
    }

    /*!***********************************************************************
    \brief
     Renders the current frame of the selected video.

    \details
     This function retrieves the video data, applies the correct transformations,
     sets up the OpenGL shader program, and renders the video as a textured quad.
    *************************************************************************/
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

    /*!***********************************************************************
    \brief
     Loads a video from a file and stores it in the video map.

    \param name The identifier for the video.
    \param filepath The file path to the MPEG video file.
    \param loop A boolean indicating whether the video should loop.
    \return True if the video was loaded successfully, false otherwise.
    *************************************************************************/
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
        int frameRate = (int)plm_get_framerate(video.plm);
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
        video.video_ctx = std::make_unique<VideoContext>(width, height);
 
        //// **Preload all frames into the texture array**
        //glBindTexture(GL_TEXTURE_2D_ARRAY, video.textureID);
        //for (int i = 0; i < video.totalFrames; ++i)
        //{
        //    // Decode the next frame
        //    plm_frame_t* frame = plm_decode_video(video.plm);
        //    if (!frame) break;

        //    // Convert to RGB
        //    plm_frame_to_rgb(frame, video.video_ctx->rgb_buffer.get(), width * 3);
        //    
        //    if (!video.video_ctx->rgb_buffer) {
        //        UME_ENGINE_ERROR("Error: RGB buffer is null at frame {0}", i);
        //    }

        //    // Upload to the 2D texture array at layer `i`
        //    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, video.video_ctx->rgb_buffer.get());

        //    UME_ENGINE_INFO("Uploaded frame {0}/{1} to texture array layer {2}", i + 1, video.totalFrames, i);
        //}

        std::thread decoding([&]()
            {
                for (int i{}; i < video.totalFrames; i++)
                {
                    // Decode next frame
                    plm_frame_t* frame = plm_decode_video(video.plm);
                    if (!frame)
                        break;
                    
                    // Convert frame into RGB Buffer
                    plm_frame_to_rgb(frame, video.video_ctx->rgb_buffer.get(), width * 3);

                    // Push Frame into thread safe queue
                    {
                        std::lock_guard<std::mutex> lock(frameQueueMutex);
                        frameQueue.push(video.video_ctx->rgb_buffer.get());
                    }

                    UME_ENGINE_INFO("Decoded frame {0}/{1}", i + 1, video.totalFrames);
                }
            });

        glBindTexture(GL_TEXTURE_2D_ARRAY, video.textureID);

        int batch = 10;
        int upload_frame = 0;

        while (upload_frame < video.totalFrames)
        {
            std::vector<GLubyte*> frame_to_load;
            {
                std::lock_guard<std::mutex> lock(frameQueueMutex);
                while (!frameQueue.empty() && frame_to_load.size() < batch)
                {
                    frame_to_load.push_back(frameQueue.front());
                    frameQueue.pop();   
                }
            }

            if (!frame_to_load.empty())
            {
                for (size_t i{}; i < frame_to_load.size(); i++)
                {
                    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, upload_frame + i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, frame_to_load[i]);
                    UME_ENGINE_INFO("Uploaded frame {0}/{1} to texture array layer {2}", upload_frame + i + 1, video.totalFrames, upload_frame + i);
                }
                upload_frame += frame_to_load.size();
            }
        }

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        // Disable looping for playback
        plm_set_loop(video.plm, false); // Handled manually in `UpdateAndRenderVideo;

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

        ECS::GetInstance().GetSystem<Audio>()->LoadSound(0, "../Assets/Video/intro-cutscene.wav", "SFX");

        videos[name] = std::move(video);  // Store the video in the map


        decoding.join();

        return true;
    }

    /*!***********************************************************************
    \brief
     Creates an OpenGL texture array for storing video frames.

    \param videoName The name of the video associated with this texture.
    \param width The width of each frame.
    \param height The height of each frame.
    \param num_frames The total number of frames in the video.
    \return The generated OpenGL texture ID.
    *************************************************************************/
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

    /*!***********************************************************************
    \brief
     Updates the video playback and renders the current frame.

    \details
     This function updates the current video’s elapsed time, checks if it should
     progress to the next frame, and handles video looping behavior. If the video
     is finished and not looping, it releases resources.
    *************************************************************************/
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

        float deltaTime = (float)g_FrameRateController.GetDeltaTime();
#ifdef _DEBUG
        UME_ENGINE_TRACE("Decoding Video - DeltaTime: {0}", deltaTime);
#endif
        Audio::GetInstance().Update(); // Update audio

        // Get the elapsed time since the last frame
        video.elapsedTime += deltaTime;

        if (ECS::GetInstance().GetSystem<Audio>()->CheckSFX())
        {
            if (!ECS::GetInstance().GetSystem<Audio>()->IsSFXPlaying(0))
            {
                ECS::GetInstance().GetSystem<Audio>()->PlaySound(0, "SFX", 0.2f);
            }
        }


        // Only update the video frame if enough time has passed
        if (video.currentFrame < video.totalFrames - 1)
        {
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

    /*!***********************************************************************
    \brief
     Sets the current active video by name.

    \param name The name of the video to be set as the current video.
    *************************************************************************/
    void VideoManager::SetCurrentVideo(const std::string& name)
    {
        if (videos.find(name) == videos.end()) 
        {
            UME_ENGINE_ERROR("Video {0} not found!", name);
            return;
        }
        currentVideo = name;
    }


    /*!***********************************************************************
    \brief
     Updates the video manager.

    \details
     This function calls `UpdateAndRenderVideo()` to ensure video playback
     continues in the update loop.
    *************************************************************************/
    void VideoManager::Update()
    {
        UpdateAndRenderVideo();  
    }

    /*!***********************************************************************
    \brief
     Skips the currently playing video.

    \details
     This function stops any associated audio and releases the video resources.
    *************************************************************************/
    void VideoManager::SkipVideo()
    {
        //UME_ENGINE_TRACE("Skipping video...");

        // Stop any playing audio
        ECS::GetInstance().GetSystem<Audio>()->DeleteSound(0, "SFX");

        // Free video resources
        Free();
    }

    /*!***********************************************************************
    \brief
     Frees the resources associated with the current video.

    \details
     This function deletes textures, destroys the PLM video object, and cleans up
     allocated memory.
    *************************************************************************/
    void VideoManager::Free()
    {
        VideoData& video = videos[currentVideo];
        ECS::GetInstance().GetSystem<Audio>()->DeleteSound(0, "SFX");

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

        video.done = true;
    }

    void VideoManager::CleanupAllVideos()
    {
        for (auto it = videos.begin(); it != videos.end(); ++it)
        {
            VideoData& video = it->second;

            // Ensure audio is cleaned up
            ECS::GetInstance().GetSystem<Audio>()->DeleteSound(0, "SFX");

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

            video.done = true;
        }

        // Clear the video map after freeing all resources
        videos.clear();
    }

}
