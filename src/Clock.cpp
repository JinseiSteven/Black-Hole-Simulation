//
// Created by StephanVisser on 1/19/2026.
//

#include "Clock.h"
#include "Config.h"


Clock::Clock() :
    m_last_frame_time(ClockType::now())
{ }

float Clock::tick() {
    const TimePoint now = ClockType::now();
    m_delta_time = std::chrono::duration<float>(now - m_last_frame_time).count();
    m_last_frame_time = now;

    m_frame_count++;
    m_time_since_fps_update += m_delta_time;
    if (m_time_since_fps_update >= Config::FPS_UPDATE_LATENCY) {
        m_current_fps = static_cast<int>(m_frame_count / m_time_since_fps_update);
        m_frame_count = 0;
        m_time_since_fps_update = 0;
    }

    return m_delta_time;
}


