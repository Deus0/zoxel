// a sun: a light source in a realm's sky, spawned by its game as the realm generates (its solar step, i.e.
//  gam/zoxel/sys/solar.c) (doc/plans/day_night.md)
//  - its day is its realm's clock (ClockTime, it stops while the realm's paused) over its DayLength, from DayStart
//  - its colour high up (ColorRGB), its sunset's low down (SecondaryColorRGB)
entity spawn_prefab_sun(ecs* world) {
    zox_prefab();
    zox_prefab_name("sun");
    zox_add(e, Sun);
    zox_setv(e, DayLength, sun_day_length);
    zox_setv(e, DayStart, sun_day_start);
    zox_setv(e, ColorRGB, sun_color);
    zox_setv(e, SecondaryColorRGB, sun_color);
    return e;
}

// a realm's sun, its colour low in the sky (sunrise, sunset)
entity spawn_sun(ecs* world, entity realm, color_rgb sunset) {
    zox_instance(prefab_sun);
    zox_name("sun");
    zox_set_parent(world, e, realm);
    zox_setv(e, SecondaryColorRGB, sunset);
    return e;
}
