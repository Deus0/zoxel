// a camera's sky light from its realm's suns (its streamer's terrain's realm), for its view (rendering's RenderView)
//  - the suns' light summed, at least the night's; the direction is the brightest's
//  - a camera without a realm (menus, viewports) keeps its own: full daylight from above
#define camera_sky_suns_max 4

void camera_sky_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(SkyTint);
    zox_sys_out(SunDirection);
    zox_sys_out(SunColor);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(SkyTint, tint);
        zox_sys_o(SunDirection, direction);
        zox_sys_o(SunColor, sun_color);
        const entity terrain = zox_get_link(world, e, StreamLink);
        const entity realm = zox_valid(terrain) ? zox_get_link(world, terrain, RealmLink) : 0;
        if (!zox_valid(realm) || !zox_has(realm, ClockTime)) {
            continue;
        }
        entity suns[camera_sky_suns_max];
        const uint count = zox_get_children_by_id(world, realm, suns, camera_sky_suns_max, zox_id(Sun));
        if (!count) {
            continue;
        }
        const double time = zox_getv(realm, ClockTime);
        float3 light = float3_zero;
        float brightest = -1;
        for (uint j = 0; j < count; j++) {
            const entity sun = suns[j];
            const float3 towards = sun_direction_at(sun_time_of_day(time, zox_getv(sun, DayLength), zox_getv(sun, DayStart)));
            const float3 color = sun_color_at(towards, zox_getv(sun, ColorRGB), zox_getv(sun, SecondaryColorRGB));
            const float3 sun_light = sun_light_at(towards, color);
            light = float3_add(light, sun_light);
            const float strength = sun_light.x + sun_light.y + sun_light.z;
            if (strength > brightest) {
                brightest = strength;
                direction->value = towards;
                sun_color->value = color;
            }
        }
        tint->value = float3_max(sky_night_tint, light);
    }
    zox_sys_on_end();
} zoxd_system(camera_sky_system);
