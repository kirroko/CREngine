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
    void VideoManager::Video_Callback(plm_t* plm, plm_frame_t* frame, void* user)
    {
        VideoContext *video_ctx = static_cast<VideoContext*>(user);
        plm_frame_to_rgb(frame,video_ctx->rgb_buffer,static_cast<int>(frame->width) * 3);

        glBindTexture(GL_TEXTURE_2D, ECS::GetInstance().GetSystem<VideoManager>()->videoTextureID);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, static_cast<int>(frame->width), static_cast<int>(frame->height),0,
            GL_RGB,GL_UNSIGNED_BYTE,video_ctx->rgb_buffer);
        glUniform2d(static_cast<GLint>(video_ctx->texture_crop_size),1.0,1.0);
        
        // VideoContext *video_ctx = static_cast<VideoContext*>(user);
        //
        // int buffer_size = static_cast<int>(frame->width) * static_cast<int>(frame->height) * 3; // 3 bytes per pixel
        //
        // // Allocate the RGB buffer on the first callback if it hasn't been allocated
        // if (!video_ctx->rgb_buffer)
        // {
        //     video_ctx->rgb_buffer = static_cast<uint8_t*>(malloc(buffer_size));
        //     if (!video_ctx->rgb_buffer)
        //     {
        //         UME_ENGINE_ERROR("Failed to allocate RGB buffer.");
        //         return;
        //     }
        // }
        //
        // uint8_t *rgb_buffer = (uint8_t*)malloc(frame->width * frame->height * 3);
        // plm_frame_to_rgb(frame,rgb_buffer,static_cast<int>(video_ctx->width)* 3);
        // video_ctx->rgb_buffer = rgb_buffer;
    }

    void VideoManager::Audio_Callback(plm_t* plm, plm_samples_t* frame, void* user)
    {
        ECS::GetInstance().GetSystem<Audio>()-> frame->interleaved
        // RingBuffer* rb = static_cast<RingBuffer*>(user);
        //
        // for (unsigned int i = 0; i < frame->count; i++)
        // {
        //     rb->buffer[rb->write_index] = frame->interleaved[i];
        //     rb->write_index = (rb->write_index + 1) % rb->capacity;
        // }
    }

    FMOD_RESULT F_CALLBACK VideoManager::pcmReadCallback(FMOD_SOUND* sound, void* data, unsigned int datalen)
    {
        float *dest = static_cast<float*>(data);
        unsigned int samples_requested = datalen / sizeof(float);

        for (unsigned int i = 0; i < samples_requested; i++)
        {
            if (rb->read_index != rb->write_index)
            {
                dest[i] = rb->buffer[rb->read_index];
                rb->read_index = (rb->read_index + 1) % rb->capacity;
            }
            else
                dest[i] = 0.0f; // fill with silence.
        }
        
        return FMOD_OK;
    }

    bool VideoManager::LoadVideo(const char* filepath)
    {
        plm = plm_create_with_filename(filepath);
        if (!plm)
        {
            UME_ENGINE_ERROR("Failed to load video: {0]",filepath);
            return false;
        }

        if (!plm_probe(plm,5000 * 1024))
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

        video_ctx = new VideoContext(800,600);
        plm_set_video_decode_callback(plm,Video_Callback,video_ctx);
        plm_set_audio_decode_callback(plm,Audio_Callback,nullptr);

        plm_set_loop(plm,true);
        plm_set_audio_enabled(plm,true);
        plm_set_audio_stream(plm,0);

        if (plm_get_num_audio_streams(plm) > 0)
        {
            FMOD_CREATESOUNDEXINFO exinfo = {};
            exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
            exinfo.length = 4096;
            exinfo.numchannels = 2;             // Stereo
            exinfo.defaultfrequency = samplerate;    // hz
            exinfo.format = FMOD_SOUND_FORMAT_PCMFLOAT;

            FMOD_RESULT result = Audio::GetInstance().pSystem->createSound(nullptr,FMOD_OPENUSER | FMOD_CREATESTREAM, &exinfo,&fmod_sound);
            if (result != FMOD_OK)
                UME_ENGINE_ERROR("FMOD error (CreateSound)");

            // Adjust the audio lead time according to the audio_spec buffer size
            plm_set_audio_lead_time(plm, static_cast<double>(exinfo.length) / samplerate);
        }
        
        videoTextureID = CreateVideoTexture(plm_get_width(plm),plm_get_height(plm));
        size_t num_pixels = static_cast<size_t>(plm_get_width(plm)) * plm_get_height(plm);
        video_ctx->rgb_buffer = static_cast<uint8_t*>(malloc(num_pixels * 3));

        video_ctx->texture_crop_size = glGetUniformLocation(ECS::GetInstance().GetSystem<Renderer>()->shaderProgram->ID,"texture_crop_size");
        
        return true;
    }

    GLuint VideoManager::CreateVideoTexture(int width, int height)
    {
        GLuint texture;
        glCreateTextures(GL_TEXTURE_2D,1,&texture);

        glBindTexture(GL_TEXTURE_2D,texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

        glUniform1i(glGetUniformLocation(ECS::GetInstance().GetSystem<Renderer>()->shaderProgram->ID, "texture_rgb"), 0);
        return texture;
    }

    void VideoManager::UpdateAndRenderVideo(plm_t* video, const GLuint& textureID)
    {
        if (!video) return;
        
        do // It'll keep playing till finish
        {
            plm_decode(plm,g_FrameRateController.GetDeltaTime());
        } while (!plm_has_ended(plm));
        
        Free();
    }

    void VideoManager::Init(int width, int height)
    {
        videoTextureID = CreateVideoTexture(width,height); // creating the video texture
        // Install the video & audio decode callbacks
        video_ctx = new VideoContext{400,600};
        rb = new RingBuffer();
        plm_set_video_decode_callback(plm,Video_Callback,video_ctx); 
        plm_set_audio_decode_callback(plm,Audio_Callback,rb);

        // Configuration for FMOD
        FMOD_CREATESOUNDEXINFO exinfo = {};
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        exinfo.length = 4096;               // buffer size
        exinfo.numchannels = 2;             // Stereo
        exinfo.defaultfrequency = 44100;    // hz
        exinfo.format = FMOD_SOUND_FORMAT_PCMFLOAT;

        FMOD_RESULT result = Audio::GetInstance().pSystem->createSound(nullptr,FMOD_OPENUSER | FMOD_CREATESTREAM, &exinfo,&fmod_sound);
        if (result != FMOD_OK)
            UME_ENGINE_ERROR("FMOD error (CreateSound)");
    }

    void VideoManager::Update()
    {
        UpdateAndRenderVideo(plm,videoTextureID);
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
