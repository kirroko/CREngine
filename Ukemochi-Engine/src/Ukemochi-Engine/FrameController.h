#pragma once
#include "Core.h"

namespace UME {

    class UME_API FrameRateController
    {
    public:
        FrameRateController();
        void Update(); // Call this method every frame
        double GetFPS() const; // Get the current FPS
        double GetDeltaTime() const;

    private:
        double m_FPS;
        double m_LastTime;
        double m_FrameCount;
        double m_DeltaTime; // Delta time between frames
        double m_FPSInterval; // Interval to update FPS (in seconds)
        double m_LastFPSUpdateTime; // Time of last FPS update
    };

    extern UME_API FrameRateController g_FrameRateController; // Declare the global instance
}