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
            : m_FPS(0.0), m_LastTime(glfwGetTime()), m_FrameCount(0), m_FPSInterval(1.0), m_LastFPSUpdateTime(glfwGetTime()), m_DeltaTime(0.0)
        {
        }

        void FrameRateController::Update()
        {
            double currentTime = glfwGetTime();
            m_FrameCount++;

            // Calculate deltaTime (difference between frames)
            m_DeltaTime = currentTime - m_LastTime;
            m_LastTime = currentTime;

            if (currentTime - m_LastFPSUpdateTime >= m_FPSInterval)
            {
                m_FPS = m_FrameCount / (currentTime - m_LastFPSUpdateTime);
                m_LastFPSUpdateTime = currentTime;
                m_FrameCount = 0;
            }
        }

        double FrameRateController::GetFPS() const { return m_FPS; }
        double FrameRateController::GetDeltaTime() const { return m_DeltaTime; }

        // Define the global instance
        FrameRateController g_FrameRateController;
   } // namespace UME