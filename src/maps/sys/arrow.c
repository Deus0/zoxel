zox_sys2(MapArrowSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerLink);
    zox_sys_out(LocalRotation2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerLink, player);
        zox_sys_o(LocalRotation2, rotation);
        if (!zox_valid(player->value)) {
            continue;
        }
        entity character = zox_get_link(world, player->value, Character);
        if (!zox_valid(character)) {
            continue;
        }
        float4 rotation3 = zox_getv(character, Rotation3D);
        float3 euler = quaternion_to_euler(rotation3);
        if (zox_maps_flip_z) {
            // euler.x = 180 * degrees_to_radians;
            // euler.y += 90 * degrees_to_radians;
            // euler.y = - euler.y + 180 * degrees_to_radians;
            // flip x axis
            euler.y += 180 * degrees_to_radians;
        }
        rotation->value = euler.y; //  + 180 * degrees_to_radians;
        if (dbg_log) {
            zox_log("Map Arrow [%s] Rotation Set [%f]", zox_getn(e), rotation->value);
        }
    }
} zox_sys_end(MapArrowSystem);
