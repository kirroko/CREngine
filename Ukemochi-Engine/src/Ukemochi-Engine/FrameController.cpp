/* Start Header
*****************************************************************/
/*!
\file       FrameController.cpp
\author     Hurng Kai Rui, h.kairui, 2301278
\par        email: h.kairui\@digipen.edu
\date       Sept 12, 2024
\brief      Implementation of the FrameRateController class, which manages frame rate calculations
             and delta time between frames in a game or graphical application. This class provides
             functionality to update frame counts, calculate frames per second (FPS), and retrieve
             delta time for smoother rendering and timing control.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "FrameController.h"
#include <GLFW/glfw3.h> // For glfwGetTime()

namespace Ukemochi {

    // Constructor initializes the FrameRateController
    FrameRateController::FrameRateController()
        : m_FPS(0.0), // Initialize FPS to 0.0
        m_LastTime(glfwGetTime()), // Set last time to current time
        m_FrameCount(0), // Initialize frame count to 0
        m_FPSInterval(1.0), // FPS update interval (1 second)
        m_LastFPSUpdateTime(glfwGetTime()), // Track last FPS update time
        m_DeltaTime(0.0) // Initialize delta time to 0.0
    {
    }

    /*!
    \brief Updates the frame rate calculations.
    This method increments the frame count, calculates the delta time since the last frame,
    and updates the FPS every specified interval (m_FPSInterval).

    The FPS is calculated based on the number of frames rendered during the interval.
    After updating the FPS, the frame count is reset to zero.
    */
    void FrameRateController::Update()
    {
        double currentTime = glfwGetTime(); // Get current time
        m_FrameCount++; // Increment frame count

        // Calculate deltaTime (time elapsed since last frame)
        m_DeltaTime = currentTime - m_LastTime;
        m_LastTime = currentTime; // Update last time to current

        // Update FPS every FPSInterval seconds
        if (currentTime - m_LastFPSUpdateTime >= m_FPSInterval)
        {
            m_FPS = m_FrameCount / (currentTime - m_LastFPSUpdateTime); // Calculate FPS
            m_LastFPSUpdateTime = currentTime; // Update last FPS update time
            m_FrameCount = 0; // Reset frame count
        }
    }

    // Gets the current FPS.
    /*!
    \brief Retrieves the current frames per second (FPS).
    \return The most recently calculated FPS value.
    */
    double FrameRateController::GetFPS() const { return m_FPS; }
    // Gets the delta time between frames.
    /*!
    \brief Retrieves the time elapsed between the last two frames.
    \return The time (in seconds) since the last frame was rendered.
    */
    double FrameRateController::GetDeltaTime() const { return m_DeltaTime; }

    // Define the global instance
    FrameRateController g_FrameRateController;

} // namespace UME