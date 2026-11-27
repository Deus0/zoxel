// zoxel's realms' suns: one, spawned in the realm's solar generation step, its old one gone (a realm generated
//  again); later its planets and their orbits (doc/plans/day_night.md)

// a realm's sunset colour, from its seed (as its biomes' colours are): most warm, some alien
static inline color_rgb zoxel_sunset_color(lint seed) {
    const uint s = seed_rand((uint) seed ^ (uint) (seed >> 32) ^ 0x5a7u);
    const float hue = seed_rangef(s, 0, 1) < 0.65f ? seed_rangef(s + 1, -25, 45) : seed_rangef(s + 2, 0, 360);
    const float3 hsv = { hue < 0 ? hue + 360 : hue, seed_rangef(s + 3, 55, 80), 100 };
    return hsv_to_color_rgb(hsv);
}

#define solar_suns_max 8

void solar_realm_spawn_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Seed);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Seed, seed);
        if (state->value != zox_generate_realm_solar) {
            continue;
        }
        entity old[solar_suns_max];
        const uint count = zox_get_children_by_id(world, e, old, solar_suns_max, zox_id(Sun));
        for (uint j = 0; j < count; j++) {
            zox_delete(old[j]);
        }
        spawn_sun(world, e, zoxel_sunset_color(seed->value));
    }
    zox_sys_on_end();
} zoxd_system(solar_realm_spawn_system);
