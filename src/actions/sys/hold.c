// NOTE: Uses slot index to equip a new action
zox_sys2(ActiveActionHoldSystem) {
    float scale = 0.125f / 2.5f;
    float3 position = (float3) { -0.125f / 4, 0.26f, 0.16f };
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActiveActionDirty);
    zox_sys_in(ActiveAction);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ActiveActionDirty, dirty);
        zox_sys_i(ActiveAction, aaction);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        byte spawn_held = zox_valid(aaction->value) && zox_has(aaction->value, Item);
        entity e2 = zox_get_child_by_id(world, e, zox_id(HeldAction));
        if (spawn_held) {
            // check if exists
            if (!e2) {
                // TODO: Spawn based on hand bone
                if (dbg_log) {
                    zox_log("Spawning Item [%s]", zox_get_name(aaction->value));
                }
                // entity e2 = spawn_cube(world, prefab_cube, float3_zero, scale);
                entity texture = zox_gett_value(aaction->value, TextureLink);
                entity e2 = spawn_cube_textured(world, prefab_cube_textured, texture, float3_zero, scale);
                zox_set_parent(world, e2, e);
                zox_add_tag(e2, HeldAction);
                zox_set(e2, LocalPosition3D, { position });
            } else {
                if (dbg_log) {
                    zox_log("Held item already existed");
                }
            }
        } else {
            if (e2) {
                if (dbg_log) {
                    zox_log("Removing Held Item [%s]", zox_get_name(e2));
                }
                zox_delete(e2);
            } else {
                if (dbg_log) {
                    zox_log("Held item did not exist");
                }
            }
        }
    }
} zox_sys_end(ActiveActionHoldSystem);
