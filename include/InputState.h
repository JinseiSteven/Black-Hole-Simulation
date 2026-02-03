//
// Created by StephanVisser on 1/20/2026.
//

#ifndef BLACK_HOLE_SIMULATION_INPUTSTATE_H
#define BLACK_HOLE_SIMULATION_INPUTSTATE_H

#define LEFT_MOUSE_FLAG 1
#define RIGHT_MOUSE_FLAG 2
#define CTRL_FLAG 4
#define ESC_FLAG 8
#define TAB_FLAG 16

// our inputstate datastruct will keep track of the input in between frames
struct InputState {
    double mouse_delta_x{0};
    double mouse_delta_y{0};
    int keys_pressed_mask{0};
    int keys_held_mask{0};
    bool consumed{false};

    void clear() {
        mouse_delta_x = 0;
        mouse_delta_y = 0;
        keys_pressed_mask = 0;
        consumed = false;
    }
    [[nodiscard]] bool consume() {
        const bool prev_consumed = consumed;
        consumed = true;

        // we also return the prev consumed value, to let it know whether it had already been consumed before
        return prev_consumed;
    }
};

#endif //BLACK_HOLE_SIMULATION_INPUTSTATE_H