double time_begin, zox_current_time, zox_delta_time = 0.0;
double frames_per_second_time = 0.0;
int frames_count, frames_per_second = 0;
double zox_current_time_check = 0.0;
const double max_zox_delta_time = 1.0 / 10.0;
byte last_updated_time = 0;
byte updating_time = 1;
byte time_pause_disabled = 0;
#define record_frames_count 500
const double zox_lag_cutoff = 4;

// NOTE: Time Pausing Disabled because it Line Renders
//          Will keep spawning and not die
//          DestroyInTimeSystem needs time for now during pause events

void disable_time_pausing() {
    // time_pause_disabled = 1;
}

void enable_time() {
    // updating_time = 1;
}

void disable_time() {
    //if (!time_pause_disabled) {
    //    updating_time = 0;
    //}
}
