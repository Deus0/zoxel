// NOTE: Uses slot index to equip a new action
zox_sys2(ActiveActionHoldSystem) {
    byte dbg_log = 0;
    // TODO: If item changes?
    float scale = 0.125f;
    float3 hand_position = (float3) { 0, 0, -scale / 2.0f - 0.01f };
    float3 body_position = (float3) { -0.125f / 4, 0.26f, 0.16f };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActiveActionDirty);
    zox_sys_in(ActiveAction);
    zox_sys_in(HandBoneLink);
    zox_sys_out(RaiseShoulder);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ActiveActionDirty, dirty);
        zox_sys_i(ActiveAction, aaction);
        zox_sys_i(HandBoneLink, hand_bone);
        zox_sys_o(RaiseShoulder, raise);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        byte new_raise = zox_valid(aaction->value);
        if (raise->value != new_raise) {
            raise->value = zox_valid(aaction->value); // raise arm if proper action
            if (dbg_log) {
                zox_log("[%s] is Raising their arm for [%s]", zox_getn(e), zox_getn(aaction->value));
            }
        }
        byte spawn_held = zox_valid(aaction->value) && zox_has(aaction->value, Item);
        entity bone_parent;
        if (zox_valid(hand_bone->value)) {
            bone_parent = hand_bone->value;
        } else {
            bone_parent = e;
            zox_logw("Character [%s] Missing Hand", zox_get_name(e));
        }
        entity e2 = zox_get_child_by_id(world, bone_parent, zox_id(HeldAction));
        if (zox_valid(e2)) {
            if (dbg_log) {
                zox_log("Removing Held Item [%s]", zox_get_name(e2));
            }
            zox_delete(e2);
        } else {
            if (dbg_log) {
                zox_log("No HeldAction Model Found on [%s]", zox_get_name(e));
            }
        }
        if (!spawn_held) {
            continue;
        }
        // TODO: Spawn based on hand bone
        float3 position;
        if (hand_bone->value) {
            position = hand_position;
        } else {
            position = body_position;
        }
        entity texture = zox_getv(aaction->value, TextureLink);
        if (zox_valid(texture)) {
            e2 = spawn_cube_textured(world, prefab_cube_textured, texture, float3_zero, scale);
        } else {
            e2 = spawn_cube(world, prefab_cube, float3_zero, scale);
        }
        zox_set_parent(world, e2, bone_parent);
        zox_add_tag(e2, HeldAction);
        zox_setv(e2, LocalPosition3D, position);
        zox_setv(e2, LocalScale1, scale);
        zox_add_tag(e2, DisableParentScale);
        if (dbg_log) {
            zox_log("[%s] has Spawned Held Item [%s] at [%fx%fx%f]", zox_getn(e), zox_getn(aaction->value), position.x, position.y, position.z);
        }
    }
} zox_sys_end(ActiveActionHoldSystem);
