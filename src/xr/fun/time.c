// --------------------------------------------------
// Time
// --------------------------------------------------
static float delta_time;

// NOTE: Uses static frame_state
void update_time() {
    delta_time = 0.016f;
    if (previous_display_time != 0) {
        delta_time =
            (float)(frame_state.predictedDisplayTime -
                    previous_display_time) /
            1000000000.0f;
    }
    previous_display_time =
        frame_state.predictedDisplayTime;
    if (delta_time > 0.1f) {
        delta_time = 0.1f;
    }
}