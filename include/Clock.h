//
// Created by StephanVisser on 1/19/2026.
//

#ifndef BLACK_HOLE_SIMULATION_CLOCK_H
#define BLACK_HOLE_SIMULATION_CLOCK_H

#include <chrono>

class Clock {
    using ClockType = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<ClockType>;
public:
    Clock();

    float tick();
    [[nodiscard]] float GetDeltaTime() const { return m_delta_time; };
    [[nodiscard]] int getFPS() const { return m_current_fps; };

private:
    float m_delta_time{0};
    TimePoint m_last_frame_time;

    double m_time_since_fps_update{0};
    int m_frame_count{0};
    int m_current_fps{0};
};

#endif //BLACK_HOLE_SIMULATION_CLOCK_H