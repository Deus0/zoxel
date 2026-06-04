void crosshair_set_type(ecs* world, entity e, byte new) {
    if (!zox_valid(e) || !zox_has(e, HitType)) {
        return;
    }
    byte old = zox_getv(e, HitType);
    if (old == new) {
        return;
    }
    float scale = crosshair_air_scale;
    color crosshair_fill;
    if (new == rayhit_terrain) {
        crosshair_fill = crosshair_terrain;
        scale = crosshair_terrain_scale;
    } else if (new == rayhit_block_vox) {
        crosshair_fill = crosshair_terrain;
        scale = crosshair_terrain_scale;
    } else if (new == rayhit_character) {
        crosshair_fill = crosshair_target;
        scale = crosshair_target_scale;
    } else {
        crosshair_fill = crosshair_air;
    }
    if (zox_valid(e)) {
        zox_set(e, HitType, { new });
        zox_set(e, OutlineColor, { crosshair_fill });
        zox_set(e, GenerateTexture, { zox_dirty_trigger });
        zox_set(e, Scale1D, { scale });
    }
}

zox_sys2(RaycastCrosshairSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaycastVoxelData);
    zox_sys_in(PlayerLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RaycastVoxelData, data);
        zox_sys_i(PlayerLink, player);
        if (!zox_valid(player->value) || !zox_has(player->value, CanvasLink)) {
            zox_loge("Player has no canvas");
            continue;
        }
        zox_geter_value(player->value, CanvasLink, entity, canvas);
        if (!zox_valid(canvas)) {
            continue;
        }
        entity crosshair = zox_get_child_by_id(world, canvas, zox_id(Crosshair));
        if (zox_valid(crosshair)) {
            crosshair_set_type(world, crosshair, data->result);
        }
    }
} zox_sys_end(RaycastCrosshairSystem);
