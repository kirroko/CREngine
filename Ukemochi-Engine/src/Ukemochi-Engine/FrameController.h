/* Start Header
*****************************************************************/
/*!
\file       FrameController.h
\author     Hurng Kai Rui, h.kairui, 2301278
\par        email: h.kairui\@digipen.edu
\date       Sept 12, 2024
\brief      Header file for the FrameRateController class

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "Core.h"

namespace Ukemochi {

    // FrameRateController class
    /*!
    \class FrameRateController
    \brief Manages frame rate calculations and provides methods to retrieve FPS and delta time.
    This class should be updated every frame to ensure accurate timing and frame rate calculations.
    */
    class UME_API FrameRateController
    {
    public:
        /*!
         \brief Constructs a FrameRateController instance.
         Initializes the FPS, last time, frame count, FPS update interval, last FPS update time,
         and delta time for frame rate calculations.
         */
        FrameRateController();
        // Updates the frame rate calculations
        /*!
        \brief Call this method every frame.
        This method increments the frame count, calculates delta time, and updates FPS every specified interval.
        */
        void Update(); // Call this method every frame
        double GetFPS() const; // Get the current FPS
        double GetDeltaTime() const; // Gets the delta time between frames
        double GetFixedDeltaTime() const; // Gets the fixed delta time
        int GetCurrentNumberOfSteps() const; // Gets the current number of steps processed.
        void SetCurrentNumberOfSteps(int steps); // Sets the current number of steps processed.

    private:
        double m_FPS;
        double m_LastTime;
        double m_FrameCount;
        double m_DeltaTime; // Delta time between frames
        double m_FPSInterval; // Interval to update FPS (in seconds)
        double m_LastFPSUpdateTime; // Time of last FPS update
        int m_CurrentNumberOfSteps; // Holds the current number of steps processed
        const double m_FixedDeltaTime = 1.0 / 60.0; // Targeted fixed delta time
    };

    extern UME_API FrameRateController g_FrameRateController; // Declare the global instance
}