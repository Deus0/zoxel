#include "clouds.c"
#include "day_night.c"
#include "sky_day.c"

void zox_tsts_sky(ecs* world) {
    zox_tsts_clouds(world);
    zox_tsts_day_night(world);
    zox_tsts_sky_day(world);
}
