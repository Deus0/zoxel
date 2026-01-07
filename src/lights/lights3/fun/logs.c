#define zox_log_lighting(...) if (zox_is_log_lighting) zox_log_time_prefix("⚡️", __VA_ARGS__);

#define zox_log_lighting_light(...) if (zox_is_log_lighting && zox_is_log_lighting_light) zox_log_time_prefix("⚡️", __VA_ARGS__);

#define zox_log_lighting_remove(...) if (zox_is_log_lighting && zox_is_log_lighting_remove) zox_log_time_prefix("⚡️", __VA_ARGS__);

#define zox_log_lighting_dark(...) if (zox_is_log_lighting && zox_is_log_lighting_dark) zox_log_time_prefix("⚡️", __VA_ARGS__);

#define zox_log_lighting_place(...) if (zox_is_log_lighting && zox_is_log_lighting_place) zox_log_time_prefix("⚡️", __VA_ARGS__);
