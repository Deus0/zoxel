// a realm's sun: a light source, a child of the realm prefab so every realm has one (doc/plans/day_night.md)
//  - its day is the realm's clock (ClockTime, it stops while the realm's paused) over its DayLength, from DayStart
entity spawn_prefab_sun(ecs* world, entity realm_prefab) {
    zox_prefab();
    zox_prefab_name("sun");
    zox_add(e, Sun);
    zox_setv(e, DayLength, sun_day_length);
    zox_setv(e, DayStart, sun_day_start);
    zox_setv(e, ColorRGB, sun_color);
    zox_set_parent(world, e, realm_prefab);
    return e;
}
