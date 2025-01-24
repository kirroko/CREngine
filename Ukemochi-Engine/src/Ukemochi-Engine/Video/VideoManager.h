#pragma once
#include "../ECS/ECS.h"

namespace Ukemochi {
    class VideoManager {
    public:
        // Play a video from the video list
        static void PlayVideo(VideoData& videoData, int index);

        // Stop the currently playing video
        static void StopVideo(VideoData& videoData);

        // Add a new video to the list
        static void AddVideo(VideoData& videoData, const std::string& name, const std::string& path);

        // Remove a video by index from the list
        static void RemoveVideo(VideoData& videoData, size_t index);

        // Get video index by name
        static int GetVideoIndex(const VideoData& videoData, const std::string& name);
    };
}