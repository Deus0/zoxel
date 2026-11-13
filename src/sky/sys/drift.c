// Clouds drift with the wind, wrapping round their realm's CloudCentre (the listening camera)
const float3 cloud_wind = { 1.6f, 0, 0.7f };    // units a second

void cloud_drift_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float delta = zox_delta_time;
    const float width = clouds_field * 2;
    zox_sys_begin();
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(Position3D, position);
        const CloudCentre* centre = zox_get(zox_get_parent(world, e), CloudCentre);
        if (!centre) {
            continue;
        }
        float3* p = &position->value;
        p->x += cloud_wind.x * delta;
        p->z += cloud_wind.z * delta;
        if (p->x - centre->value.x > clouds_field) {
            p->x -= width;
        } else if (p->x - centre->value.x < -clouds_field) {
            p->x += width;
        }
        if (p->z - centre->value.z > clouds_field) {
            p->z -= width;
        } else if (p->z - centre->value.z < -clouds_field) {
            p->z += width;
        }
    }
    zox_sys_on_end();
} zoxd_system(cloud_drift_system);
