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

    void VideoManager::RenderVideoFrame()
    {
        if (!plm) return;

        UME_ENGINE_TRACE("Rendering frame {0}/{1}", currentFrame, totalFrames);

        // Activate shader
        ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->Activate();

        // Get projection and view matrices from the camera
        glm::mat4 projection = ECS::GetInstance().GetSystem<Camera>()->getCameraProjectionMatrix();
        glm::mat4 view = ECS::GetInstance().GetSystem<Camera>()->getCameraViewMatrix();

        // Get screen dimensions (assuming full-size video)
        float screenWidth = ECS::GetInstance().GetSystem<Camera>()->viewport_size.x;
        float screenHeight = ECS::GetInstance().GetSystem<Camera>()->viewport_size.y;

        // Get video dimensions
        float videoWidth = static_cast<float>(plm_get_width(plm));
        float videoHeight = static_cast<float>(plm_get_height(plm));

        // Calculate the position for centering
        float posX = (screenWidth - videoWidth) / 2.0f;
        float posY = (screenHeight - videoHeight) / 2.0f;

        // Apply translation and scaling
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(posX, posY, 0.0f));  // Move to center
        model = glm::scale(model, glm::vec3(videoWidth, videoHeight, 1.0f)); // Scale to full size

        // Final MVP matrix
        glm::mat4 mvp = projection * view * model;

        // Pass it to the shader
        glUniformMatrix4fv(glGetUniformLocation(ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->ID, "mvp"),
            1, GL_FALSE, glm::value_ptr(mvp));

        
        // Bind VAO
        glBindVertexArray(VAO);
        // Check currently bound VAO
        GLint currentVAO = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
        UME_ENGINE_INFO("Current Bound VAO: {0}", currentVAO);

        // Check currently active shader
        GLint currentProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
        UME_ENGINE_INFO("Current Active Shader Program: {0}", currentProgram);

        // Draw the quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Unbind
        glBindVertexArray(0);
    }


    void VideoManager::Video_Callback(plm_t* plm, plm_frame_t* frame, void* user)
    {
        VideoContext *video_ctx = static_cast<VideoContext*>(user);
        UME_ENGINE_TRACE("Video Callback Triggered! Frame Width: {0}, Height: {1}", frame->width, frame->height);

        plm_frame_to_rgb(frame,video_ctx->rgb_buffer,static_cast<int>(frame->width) * 3);
        
        if (!video_ctx->rgb_buffer)
        {
            UME_ENGINE_ERROR("Error: Rgb_buffer is nullptr for frame");
            return;
        }
        UME_ENGINE_TRACE("RGB Buffer Populated. Uploading frame to OpenGL...");
        glBindTexture(GL_TEXTURE_2D_ARRAY, ECS::GetInstance().GetSystem<VideoManager>()->videoTextureID);
        glUniform1i(glGetUniformLocation(ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->ID, "frameIndex"),
            ECS::GetInstance().GetSystem<VideoManager>()->currentFrame);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, ECS::GetInstance().GetSystem<VideoManager>()->currentFrame, 
static_cast<int>(frame->width), static_cast<int>(frame->height), 1, GL_RGB, GL_UNSIGNED_BYTE, video_ctx->rgb_buffer);

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    }

    void VideoManager::Audio_Callback(plm_t* plm, plm_samples_t* frame, void* user)
    {
        if (frame->count > 0) {
            Audio::GetInstance().playStereoSound(frame->interleaved, frame->count);
        }
    }

    bool VideoManager::LoadVideo(const char* filepath)
    {
        /*plm = plm_create_with_filename(filepath);
        if (!plm)
        {
            UME_ENGINE_ERROR("Failed to load video: {0]", filepath);
            return false;
        }

        if (!plm_probe(plm, 5000 * 1024))
        {
            UME_ENGINE_ERROR("No MPEG video or audio streams found in {0}", filepath);
            return false;
        }

        int samplerate = plm_get_samplerate(plm);
        UME_ENGINE_INFO("Opened {0} - framerate: {1}, samplerate: {2}, duration: {3}",
            filepath,
            plm_get_framerate(plm),
            plm_get_samplerate(plm),
            plm_get_duration(plm));

        video_ctx = new VideoContext(800, 600);
        plm_set_video_decode_callback(plm, Video_Callback, video_ctx);
        plm_set_audio_decode_callback(plm, Audio_Callback, nullptr);

        plm_set_loop(plm, true);
        plm_set_audio_enabled(plm, true);
        plm_set_audio_stream(plm, 0);


        ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->Activate();
        glm::mat4 projection = glm::ortho(0.f, static_cast<float>(Application::Get().GetWindow().GetWidth()), 0.f, static_cast<float>(Application::Get().GetWindow().GetHeight()), -1.f, 1.f);
        ECS::GetInstance().GetSystem<Renderer>()->video_shader_program->setMat4("projection",projection);
        
        
        int frames = static_cast<int>(plm_get_framerate(plm) * plm_get_duration(plm));
        CreateVideoTexture(plm_get_width(plm),plm_get_height(plm), frames);
        size_t num_pixels = static_cast<size_t>(plm_get_width(plm)) * plm_get_height(plm);
        video_ctx->rgb_buffer = static_cast<uint8_t*>(malloc(num_pixels * 3));

        video_ctx->texture_crop_size = glGetUniformLocation(ECS::GetInstance().GetSystem<Renderer>()->shaderProgram->ID,"texture_crop_size");
        
        return true;*/

        plm = plm_create_with_filename(filepath);
        if (!plm)
        {
            UME_ENGINE_ERROR("Failed to load video: {0}", filepath);
            return false;
        }

        if (!plm_probe(plm, 5000 * 1024))
        {
            UME_ENGINE_ERROR("No MPEG video or audio streams found in {0}", filepath);
            return false;
        }

        int width = plm_get_width(plm);
        int height = plm_get_height(plm);
        int frameRate = plm_get_framerate(plm);
        int numFrames = static_cast<int>(plm_get_duration(plm) * frameRate); // Total frames

        UME_ENGINE_INFO("Total expected frames to load: {0}", numFrames);

        UME_ENGINE_INFO("Opened {0} - framerate: {1}, duration: {2}, total frames: {3}",
            filepath, frameRate, plm_get_duration(plm), numFrames);

        // Allocate storage for texture array
        CreateVideoTexture(width, height, numFrames);

        // Allocate RGB buffer
        video_ctx = new VideoContext(width, height);
        video_ctx->rgb_buffer = static_cast<uint8_t*>(malloc(width * height * 3));

        // **Preload all frames into the texture array**
        glBindTexture(GL_TEXTURE_2D_ARRAY, videoTextureID);
        for (int i = 0; i < numFrames; ++i)
        {
            // Decode the next frame
            plm_frame_t* frame = plm_decode_video(plm);
            if (!frame) break;

            // Convert to RGB
            plm_frame_to_rgb(frame, video_ctx->rgb_buffer, width * 3);

            if (!video_ctx->rgb_buffer) {
                UME_ENGINE_ERROR("Error: RGB buffer is null at frame {0}", i);
            }

            // Upload to the 2D texture array at layer `i`
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, video_ctx->rgb_buffer);

            //UME_ENGINE_INFO("Uploaded frame {0}/{1} to texture array layer {2}", i + 1, numFrames, i);
        }
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        // Enable looping for playback
        //plm_set_loop(plm, true);
        totalFrames = static_cast<int>(plm_get_framerate(plm) * plm_get_duration(plm)); // Calculate total frames

        int storedLayers = 0;
        glBindTexture(GL_TEXTURE_2D_ARRAY, videoTextureID);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &storedLayers);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        UME_ENGINE_INFO("Total stored frames in OpenGL texture array: {0}", storedLayers);

        // Debug check
        if (storedLayers != numFrames)
        {
            UME_ENGINE_ERROR("Mismatch! Expected {0} frames, but OpenGL stored {1}.", numFrames, storedLayers);
            return false;
        }

        float videoWidth = static_cast<float>(plm_get_width(plm));
        float videoHeight = static_cast<float>(plm_get_height(plm));

        // **Setup VAO & VBO once**
        float vertices[] = {
            // Positions (X, Y, Z)    // Colors       // Texture Coords  // Frame Index
            0.0f,         0.0f,        0.f, 1.0f,1.0f,1.0f,   0.0f, 0.0f,    0,  // Bottom-left
            videoWidth,   0.0f,        0.f, 1.0f,1.0f,1.0f,   1.0f, 0.0f,    0,  // Bottom-right
            0.0f,         videoHeight, 0.f,  1.0f,1.0f,1.0f,   0.0f, 1.0f,    0,  // Top-left
            videoWidth,   videoHeight, 0.f,  1.0f,1.0f,1.0f,   1.0f, 1.0f,    0   // Top-right
        };



        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);


        if (VAO == 0 || VBO == 0)
        {
            UME_ENGINE_ERROR("VAO or VBO creation failed!");
        }
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // Color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // UV
        glEnableVertexAttribArray(2);
        glVertexAttribIPointer(3, 1, GL_INT, 9 * sizeof(float), (void*)(8 * sizeof(float))); // Frame Index
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);

        return true;
    }

    Texture VideoManager::CreateVideoTexture(int width, int height, int num_frames)
    {
        /*glGenTextures(1, &videoTextureID);
        glBindTexture(GL_TEXTURE_2D_ARRAY,videoTextureID);

        if (!videoTextureID)
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

        UME_ENGINE_TRACE("Created Video Texture - ID: {0}, Width: {1}, Height: {2}, Frames: {3}", videoTextureID, width, height, num_frames);
        return videoTextureID;*/

        // Use the Texture class to create a 2D array texture
        Texture newTexture(GL_TEXTURE_2D_ARRAY, width, height, GL_RGB, GL_UNSIGNED_BYTE);

        UME_ENGINE_TRACE("Created Video Texture - Width: {0}, Height: {1}, Frames: {2}", width, height, num_frames);

        return newTexture;
    }

    void VideoManager::UpdateAndRenderVideo(plm_t* video)
    {
        if (!video)
        {
            UME_ENGINE_ERROR("Video object is null!");
            return;
        }
        float deltaTime = g_FrameRateController.GetDeltaTime();
        UME_ENGINE_TRACE("Decoding Video - DeltaTime: {0}", deltaTime);

        plm_decode(plm, deltaTime);

        Audio::GetInstance().Update(); //update audio

        // Update the current frame index (ensuring it loops correctly)
        currentFrame = (currentFrame + 1) % totalFrames;
        
        RenderVideoFrame();
        if (plm_has_ended(plm))
            Free();
        
        // if (!video) return;
        // currentFrame = -1;
        // do // It'll keep playing till finish
        // {
        //     ++currentFrame;
        //     plm_decode(plm,g_FrameRateController.GetDeltaTime());
        // } while (!plm_has_ended(plm));
        //
        // Free();
    }

    void VideoManager::Init(int width, int height)
    {
        // videoTextureID = CreateVideoTexture(width,height, TODO); // creating the video texture
        // Install the video & audio decode callbacks
        video_ctx = new VideoContext{400,600};
        rb = new RingBuffer();
        plm_set_video_decode_callback(plm,Video_Callback,video_ctx); 
        plm_set_audio_decode_callback(plm,Audio_Callback,rb);



        //fmod
        FMOD_CREATESOUNDEXINFO exinfo;

        // Create extended sound info struct
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);   // Size of the struct
        exinfo.numchannels = 2;                                // Number of channels in the sound
        exinfo.defaultfrequency = 44100;                            // Playback rate of sound
        exinfo.format = FMOD_SOUND_FORMAT_PCM16;          // Data format of sound

        FMOD_RESULT result = Audio::GetInstance().pSystem->createSound(nullptr,FMOD_OPENUSER | FMOD_CREATESTREAM, &exinfo,&Audio::GetInstance().pvideosound);
        if (result != FMOD_OK)
            UME_ENGINE_ERROR("FMOD error (CreateSound)");

    }

    void VideoManager::Update()
    {
        UpdateAndRenderVideo(plm);
    }

    void VideoManager::Free() const
    {
        if (plm)
            plm_destroy(plm);

        if (video_ctx->rgb_buffer)
            free(video_ctx->rgb_buffer);

        if (rb->buffer)
            free(rb->buffer);

        delete video_ctx;
        delete rb;
    }
}
