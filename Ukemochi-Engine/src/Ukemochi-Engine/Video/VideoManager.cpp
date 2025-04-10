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

#include <complex.h>

#include "Ukemochi-Engine/FrameController.h"
#include "Ukemochi-Engine/Graphics/Renderer.h"
#include "Ukemochi-Engine/Job/JobSystem.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"

namespace Ukemochi {

    /*!***********************************************************************
    \brief
     Checks if a video is playing.

    \return True if the video is playing, false otherwise.
    *************************************************************************/
    bool VideoManager::IsVideoPlaying() const
    {
        // Check is a video is selected
        if (currentVideo.empty())
            return false;

        // Find the video in the map
        auto it = videos.find(currentVideo);
        if (it == videos.end())
            return false;

        // Get reference to the video data
        const VideoData& video = it->second;

        // Check if the video is still playing
        return (video.plm != nullptr && !video.done && video.currentFrame < video.totalFrames);
    }

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

    /**
     * @brief Check if video exist
     * @param videoName name of the video
     * @return true if video exist
     */
	bool VideoManager::VideoExist(const std::string& videoName)
	{
		return videos.find(videoName) != videos.end();
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
		auto shaderID = ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->ID;

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

        // Update textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, video.tex_y);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video.loadingContext.jobParams[video.currentFrame].width, video.loadingContext.jobParams[video.currentFrame].height, GL_RED, GL_UNSIGNED_BYTE, video.loadingContext.jobParams[video.currentFrame].y_buffer);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, video.tex_cb);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video.loadingContext.jobParams[video.currentFrame].width / 2, video.loadingContext.jobParams[video.currentFrame].height / 2, GL_RED, GL_UNSIGNED_BYTE, video.loadingContext.jobParams[video.currentFrame].cb_buffer);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, video.tex_cr);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, video.loadingContext.jobParams[video.currentFrame].width / 2, video.loadingContext.jobParams[video.currentFrame].height / 2, GL_RED, GL_UNSIGNED_BYTE, video.loadingContext.jobParams[video.currentFrame].cr_buffer);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, video.tex_y);
        glUniform1i(glGetUniformLocation(shaderID, "tex_y"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, video.tex_cb);
        glUniform1i(glGetUniformLocation(shaderID, "tex_cb"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, video.tex_cr);
        glUniform1i(glGetUniformLocation(shaderID, "tex_cr"), 2);

        // Draw the quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Unbind
        glBindVertexArray(0);
        //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    /*!***********************************************************************
    \brief
     Loads a video from a file and stores it in the video map.

    \param name The identifier for the video.
    \param filepath The file path to the MPEG video file.
    \param loop A boolean indicating whether the video should loop.
    \return True if the video was loaded successfully, false otherwise.
    *************************************************************************/
    bool VideoManager::LoadVideo(const std::string& name, const char* filepath, bool loop, bool async)
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

        int frameRate = (int)plm_get_framerate(video.plm);
        video.width = plm_get_width(video.plm);
        video.height = plm_get_height(video.plm);
        video.totalFrames = static_cast<int>(plm_get_duration(video.plm) * frameRate); // Total frames
        video.frameDuration = (double)(1.0 / video.totalFrames) * plm_get_duration(video.plm); // Compute the required time per frame
        video.loop = loop;

#ifdef _DEBUG
        UME_ENGINE_INFO("Total expected frames to load: {0}", video.totalFrames);

        UME_ENGINE_INFO("Opened {0} - framerate: {1}, duration: {2}, total frames: {3}",
            filepath, frameRate, plm_get_duration(video.plm), video.totalFrames);
#endif

        // Allocate storage for texture array
        //video.textureID = CreateVideoTexture(name, width, height, video.totalFrames);

        // Allocate the context on the heap, contains the buffer and jobParams
        auto context = std::make_shared<VideoLoadingContext>(video.totalFrames);
 
        static auto frame_to_rgb = [](uintptr_t param)
        {
            VideoDecodeJobParams* jobParams = reinterpret_cast<VideoDecodeJobParams*>(param);
            plm_t* plm = plm_create_with_filename(jobParams->filename); // decoder
            if (!plm)
            {
                UME_ENGINE_ERROR("Failed to load video: {0}", jobParams->filename);
                return;
            }
            if (!plm_probe(plm, 5000 * 1024))
            {
                UME_ENGINE_ERROR("No MPEG video or audio streams found in {0}", jobParams->filename);
                return;
            }

            double frameTime = jobParams->frameIndex * jobParams->frameDuration;
            plm_seek(plm, frameTime, 1);

            if (plm_frame_t* frame = plm_decode_video(plm))
            {
                jobParams->width = frame->y.width;
				jobParams->height = frame->y.height;
                jobParams->y_buffer = new uint8_t[frame->y.width * frame->y.height];
                jobParams->cr_buffer = new uint8_t[frame->cr.width * frame->cr.height];
                jobParams->cb_buffer = new uint8_t[frame->cb.width * frame->cb.height];
                // Store Y plane
                memcpy(jobParams->y_buffer, frame->y.data, frame->y.width * frame->y.height);
                // Store Cr plane
                memcpy(jobParams->cr_buffer, frame->cr.data, frame->cr.width * frame->cr.height);
                // Store Cb plane
                memcpy(jobParams->cb_buffer, frame->cb.data, frame->cb.width * frame->cb.height);
            }
            else
                UME_ENGINE_ERROR("Failed to decode video");

            plm_destroy(plm);
        };

        // Pre-allocate job parameters and declarations
        std::vector<job::Declaration> jobDecls(video.totalFrames);
        
        // Crafting the jobs
        for (int i = 0; i < video.totalFrames; ++i)
        {
            // Decode the next frame
            context->jobParams[i].filename = const_cast<char*>(filepath);
            context->jobParams[i].width = video.width;
            context->jobParams[i].frameIndex = i;
            context->jobParams[i].frameDuration = video.frameDuration;

            jobDecls[i].m_pEntry = frame_to_rgb;
            jobDecls[i].m_param = reinterpret_cast<uintptr_t>(&context->jobParams[i]);
            jobDecls[i].m_priority = job::Priority::NORMAL;
        }

        if (async)
        {
            Application::Get().SetPaused(true); // Don't run the system, render video loading till done.
            job::KickJobs(static_cast<int>(jobDecls.size()), jobDecls.data());
            video.loadingContext = std::move(*context);
            videos[name] = std::move(video);
            return true;
        }

        job::KickJobsAndWait(static_cast<int>(jobDecls.size()), jobDecls.data());

        // Initialization
        glGenTextures(1, &video.tex_y);
        glBindTexture(GL_TEXTURE_2D, video.tex_y);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, context->jobParams[0].width, context->jobParams[0].height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &video.tex_cb);
        glBindTexture(GL_TEXTURE_2D, video.tex_cb);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, context->jobParams[0].width / 2, context->jobParams[0].height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &video.tex_cr);
        glBindTexture(GL_TEXTURE_2D, video.tex_cr);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, context->jobParams[0].width / 2, context->jobParams[0].height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Unbind textures
        glBindTexture(GL_TEXTURE_2D, 0);

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

		video.loadingContext = std::move(*context); // Store the context in the video data
        videos[name] = std::move(video);  // Store the video in the map

        return true;
    }

    bool VideoManager::FinishLoadingVideo(VideoData& video)
    {
        if (!video.plm)
            return false;
        if (video.loaded)
            return true;

        video.loaded = true;

        // Initialization
        glGenTextures(1, &video.tex_y);
        glBindTexture(GL_TEXTURE_2D, video.tex_y);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, video.loadingContext.jobParams[0].width, video.loadingContext.jobParams[0].height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &video.tex_cb);
        glBindTexture(GL_TEXTURE_2D, video.tex_cb);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, video.loadingContext.jobParams[0].width / 2, video.loadingContext.jobParams[0].height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &video.tex_cr);
        glBindTexture(GL_TEXTURE_2D, video.tex_cr);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, video.loadingContext.jobParams[0].width / 2, video.loadingContext.jobParams[0].height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Unbind textures
        glBindTexture(GL_TEXTURE_2D, 0);

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
     This function updates the current video�s elapsed time, checks if it should
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
        ECS::GetInstance().GetSystem<Audio>()->GetInstance().Update(); // Update audio

        // Get the elapsed time since the last frame
        video.elapsedTime += deltaTime;

        if (ECS::GetInstance().GetSystem<Audio>()->GetInstance().CheckSFX())
        {
            if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(0))
            {
                //ECS::GetInstance().GetSystem<Audio>()->GetInstance().PlaySound(0, "SFX", 0.2f);
            }
        }
		if (ECS::GetInstance().GetSystem<VideoManager>()->currentVideo == "cutscene")
		{
            if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
            {
                auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

                if (Application::Get().IsPaused == false)
                {
                    // Play BGM at 0.2 volume
                    if (audioM.GetMusicIndex("Wind_BGM") != -1)
                    {
                        int windBgmIndex = audioM.GetMusicIndex("Wind_BGM");
                        if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsMusicPlaying(windBgmIndex))
                        {
                            audioM.PlayMusic(windBgmIndex);
                            // Set volume for Wind_BGM
                            ECS::GetInstance().GetSystem<Audio>()->GetInstance().SetAudioVolume(windBgmIndex, 0.4f, "Music");
                        }
                    }
                }
            }
		}
        if (ECS::GetInstance().GetSystem<VideoManager>()->videos["cutscene"].done)
        {
            if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
            {
                auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                audioM.StopMusic(audioM.GetMusicIndex("Wind_BGM"));
            }
        }

        if (ECS::GetInstance().GetSystem<VideoManager>()->currentVideo == "before_boss")
        {
                if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
                {
                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
					audioM.StopMusic(audioM.GetMusicIndex("BGM"));
                    // Play BGM at 0.4 volume
                    if (audioM.GetMusicIndex("PreBoss_BGM") != -1)
                    {
                        int PreBoss_BGMIndex = audioM.GetMusicIndex("PreBoss_BGM");
                        if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsMusicPlaying(PreBoss_BGMIndex))
                        {
                            audioM.PlayMusic(PreBoss_BGMIndex);
                            ECS::GetInstance().GetSystem<Audio>()->GetInstance().SetAudioVolume(PreBoss_BGMIndex, 0.4f, "Music");
                        }
                    }
                }
        }

        if (ECS::GetInstance().GetSystem<VideoManager>()->currentVideo == "after_boss")
        {
            if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
            {
                auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                audioM.StopMusic(audioM.GetMusicIndex("BGM"));
                // Play BGM at 0.4 volume
                if (audioM.GetMusicIndex("AfterBoss_BGM") != -1)
                {
                    int AfterBoss_BGMIndex = audioM.GetMusicIndex("AfterBoss_BGM");
                    if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsMusicPlaying(AfterBoss_BGMIndex))
                    {
                        audioM.PlayMusic(AfterBoss_BGMIndex);
                        ECS::GetInstance().GetSystem<Audio>()->GetInstance().SetAudioVolume(AfterBoss_BGMIndex, 0.4f, "Music");
                    }
                }
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
                video.done = true;
                // Free(); // Free resources if not looping 
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
        //ECS::GetInstance().GetSystem<Audio>()->DeleteSound(0, "SFX");
		ECS::GetInstance().GetSystem<Audio>()->GetInstance().StopAllSound();
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
        if (currentVideo.empty()) return;

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

		for (auto& jobParam : video.loadingContext.jobParams)
		{
			delete[] jobParam.y_buffer;
			delete[] jobParam.cr_buffer;
			delete[] jobParam.cb_buffer;
		}

        //for (auto& buffer : video.loadingContext.rgb_buffer)
        //    buffer.reset();

        //video.loadingContext.rgb_buffer.clear();
        video.loadingContext.jobParams.clear();

        video.done = false;
        videos[currentVideo].loaded = false;
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
