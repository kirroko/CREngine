#include "PreCompile.h"
#include "FrameController.h"
#include <GLFW/glfw3.h> // For glfwGetTime()

namespace UME{

    // Initializes the FPS to 0.0 
    // Sets m_LastTime to the current time using glfwGetTime(), 
    // which returns the time in seconds since the application started This tracks when the last frame was rendered.
    // Initializes the frame count to 0. This counts how many frames have been rendered within a given interval.
    // m_LastFPSUpdateTime(glfwGetTime()): Tracks the last time the FPS was updated, starting with the current time.
    FrameRateController::FrameRateController()
        : m_FPS(0.0), m_LastTime(glfwGetTime()), m_FrameCount(0), m_FPSInterval(1.0), m_LastFPSUpdateTime(glfwGetTime())
    {
    }

    void FrameRateController::Update()
    {
        double currentTime = glfwGetTime(); // Get the current time in seconds
        m_FrameCount++; //Increment frame count for each rendered frame

        if (currentTime - m_LastFPSUpdateTime >= m_FPSInterval) //Checks if 1 second (m_FPSInterval) has passed since the last FPS update
        {
            // Calculates the FPS by dividing the total frames (m_FrameCount) by the elapsed time since the last FPS update.
            m_FPS = m_FrameCount / (currentTime - m_LastFPSUpdateTime);
            // Resets m_LastFPSUpdateTime to the current time for the next FPS calculation interval.
            m_LastFPSUpdateTime = currentTime;
            // Resets the frame count to zero for the next interval
            m_FrameCount = 0;
        }
    }

    double FrameRateController::GetFPS() const
    {
        return m_FPS;
    }

    // Define the global instance
    FrameRateController g_FrameRateController;
}