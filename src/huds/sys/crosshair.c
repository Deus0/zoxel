void crosshair_set_type(ecs* world, entity e, byte new) {
    if (!zox_valid(e) || !zox_has(e, HitType)) {
        return;
    }
    zox_geter_value(e, HitType, byte, old);
    if (old == new) {
        return;
    }
    color c;
    if (new == rayhit_none) {
        c = crosshair_air;
    } else if (new == rayhit_terrain) {
        c = crosshair_terrain;
    } else if (new == rayhit_block_vox) {
        c = crosshair_terrain;
    } else if (new == rayhit_character) {
        c = crosshair_target;
    } else {
        c = crosshair_air;
    }
    /*zox_geter_value(e, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        zox_loge("Canvas invalid");
    }*/
    entity crosshair = e; // zox_get_child_by_id(world, canvas, zox_id(Crosshair));
    if (zox_valid(crosshair)) {
        zox_set(crosshair, HitType, { new });
        zox_set(crosshair, OutlineColor, { c });
        zox_set(crosshair, GenerateTexture, { zox_dirty_trigger });
    }
}

zox_sys2(RaycastCrosshairSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaycastVoxelData);
    zox_sys_in(PlayerLink);
    // [in] layouts2.CanvasLink
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
