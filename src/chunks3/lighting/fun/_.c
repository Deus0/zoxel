#define zox_log_lighting(...) if (zox_is_log_lighting) zox_log_time_prefix("⚡️", __VA_ARGS__);

#define zox_log_lighting_light(...) if (zox_is_log_lighting && zox_is_log_lighting_light) zox_log_time_prefix("⚡️", __VA_ARGS__);

#define zox_log_lighting_dark(...) if (zox_is_log_lighting && zox_is_log_lighting_light) zox_log_time_prefix("⚡️", __VA_ARGS__);

#include "prefab.c"
#include "fetch.c"
#include "light_beam.c"
#include "light_flood.c"
#include "dark_flood.c"
#include "dark_beam.c"