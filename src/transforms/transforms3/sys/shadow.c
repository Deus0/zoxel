/*zox_sys2(ShadowPositionSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ShadowLink);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ShadowLink, link);
        zox_sys_o(Position3D, position);
        if (!zox_valid(link->value)) {
            // zox_logw("link is invalid in Shadow Position System");
            continue;
        }
        set_position_from_parents(world, link->value, &position->value, float3_zero);
    }
} zox_sys_end(ShadowPositionSystem);
*/
