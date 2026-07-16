zox_sys2(MapArrowSystem) {
    // byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerLink);
    zox_sys_out(Rotation2D);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(PlayerLink, player);
        zox_sys_o(Rotation2D, rotation);
        if (!zox_valid(player->value)) {
            continue;
        }
        entity character = zox_getv(player->value, CharacterLink);
        if (!zox_valid(character)) {
            continue;
        }
        float4 rotation3 = zox_getv(character, Rotation3D);
        float3 euler = quaternion_to_euler(rotation3);
        if (zox_maps_flip_z) {
            //  euler.x = 180 * degrees_to_radians;
            // euler.y += 90 * degrees_to_radians;
            euler.y = - euler.y + 180 * degrees_to_radians;
        }
        rotation->value = euler.y; //  + 180 * degrees_to_radians;
    }
} zox_sys_end(MapArrowSystem);
