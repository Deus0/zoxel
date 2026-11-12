// The fps display (z): the current fps, and its min and max over the last fps_display_history seconds
//  - i.e. "60 [55-62]", a low min shows hitches the current fps hides
#define fps_display_history 10
int fps_display_samples[fps_display_history];
int fps_display_samples_count = 0;
int fps_display_samples_next = 0;

static inline void add_fps_display_sample(int fps) {
    fps_display_samples[fps_display_samples_next] = fps;
    fps_display_samples_next = (fps_display_samples_next + 1) % fps_display_history;
    if (fps_display_samples_count < fps_display_history) {
        fps_display_samples_count++;
    }
}

void FpsDisplaySystem(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    double frame_rate_update_speed = 1.0;
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextData);
    zox_sys_out(FPSDisplayTicker);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(FPSDisplayTicker, tick);
        zox_sys_o(TextData, data);
        tick->value -= delta_time;
        if (tick->value > 0) {
            continue;
        }
        tick->value += frame_rate_update_speed;
        if (tick->value <= -frame_rate_update_speed) {
            tick->value = 0;
        }
        // one sample per update (per second), shared if there's more than one display
        if (i == 0) {
            add_fps_display_sample(frames_per_second);
        }
        int min = frames_per_second;
        int max = frames_per_second;
        for (int j = 0; j < fps_display_samples_count; j++) {
            min = int_min(min, fps_display_samples[j]);
            max = int_max(max, fps_display_samples[j]);
        }
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%i [%i-%i]", frames_per_second, min, max);
        if (!is_zext(data, buffer)) {
            set_zext(data, buffer);
            zox_add(e, Dirty);
            if (dbg_log) {
                zox_log("FPS updated [%s]", buffer);
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(FpsDisplaySystem);
