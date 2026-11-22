extern entity spawn_item_mesh(ecs*, entity, entity, float);

// a held item's turn in the (right) hand's rest space: its up (+y) along the thumb (-x), its front (-z) along the
//  fingers (-y), so it's upright in a fist thumb up (bones/ik ik_hand_rest_to_target)
static inline float4 held_item_rotation() {
    return ik_turn_to(float3_up, (float3) { 0, 0, -1 }, (float3) { -1, 0, 0 }, (float3) { 0, -1, 0 });
}

// NOTE: Uses slot index to equip a new action
void active_action_hold_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    // TODO: If item changes?
    float scale = 0.125f;
    float3 hand_position = (float3) {
        0,
        0,
        -scale / 2.0f - 0.01f
    };
    float3 body_position = (float3) {
        -0.125f / 4,
        0.26f,
        0.16f
    };
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
        entity skeleton = zox_get_link(world, e, SkeletonLink);
        if (!zox_valid(skeleton) || !zox_has(skeleton, RaiseShoulder)) {
            continue;
        }
        RaiseShoulder* raise = zox_getm(skeleton, RaiseShoulder);
        byte new_raise = zox_valid(aaction->value);
        if (raise->value != new_raise) {
            raise->value = zox_valid(aaction->value);
            // raise arm if proper action
            if (dbg_log) {
                zox_log("[%s] is Raising their arm for [%s]",
                    zox_getn(e),
                    zox_getn(aaction->value));
            }
        }
        byte spawn_held =
            zox_valid(aaction->value) &&
            zox_has(aaction->value, Item);
        entity hand = zox_get_link(world, skeleton, HandBoneLink);
        entity bone_parent;
        if (zox_valid(hand)) {
            bone_parent = hand;
        } else {
            bone_parent = e;
            zox_logw("Character [%s] Missing Hand",
                zox_getn(e));
        }
        entity e2 = zox_get_child_by_id(
            world,
            bone_parent,
            zox_id(HeldAction));
        if (zox_valid(e2)) {
            if (dbg_log) {
                zox_log("Removing Held Item [%s]",
                    zox_getn(e2));
            }
            zox_delete(e2);
        } else {
            if (dbg_log) {
                zox_log("No HeldAction Model Found on [%s]",
                    zox_getn(e));
            }
        }
        // its guard further out while it holds an item (bones/dat/swings.c swing_guard_held)
        if (spawn_held && !zox_has(skeleton, HoldingItem)) {
            zox_add(skeleton, HoldingItem);
        } else if (!spawn_held && zox_has(skeleton, HoldingItem)) {
            zox_remove(skeleton, HoldingItem);
        }
        if (!spawn_held) {
            continue;
        }
        // TODO: Spawn based on hand bone
        float3 position = zox_valid(hand) ?
            hand_position :
            body_position;
        entity meta = zox_getp(world, aaction->value);
        // in the palm: the hand rests in the anatomical position (its palm +z, its fingers -y, the right thumb -x,
        //  bones/ik ik_hand_rest_to_target), the item upright along the thumb, its front along the fingers, out of
        //  the palm by half its depth (a flat cookie sits closer than a block); it sat out of the hand's back (-z)
        float4 rotation = quaternion_identity;
        if (zox_valid(hand)) {
            rotation = held_item_rotation();
            const float3 size = item_mesh_size(world, meta, scale);
            const float3 turned = float4_rotate_float3(rotation, size);
            position = (float3) { 0, 0, fabsf(turned.z) / 2.0f + 0.01f };
            // its voxels' middle on the hand (a model needn't fill its grid)
            position = float3_add(position, float4_rotate_float3(rotation, item_mesh_offset(world, meta, scale)));
            // in the hand, not at its pivot: the bone sits at its joint (the wrist), its part's middle off it (at
            //  rest, so in the bone's own space), the item was on the wrist
            if (zox_has(hand, BoneCentre) && zox_has(hand, BonePosition)) {
                position = float3_add(position, float3_sub(zox_getv(hand, BoneCentre), zox_getv(hand, BonePosition)));
            }
        }
        // entity texture = zox_get_link(world, aaction->value, TextureLink);
        entity mesh = spawn_item_mesh(world, bone_parent, meta, scale);
        /*if (zox_valid(texture)) {
            e2 = spawn_cube_textured(
                world,
                prefab_cube_textured,
                texture,
                float3_zero,
                scale);
        } else {
            e2 = spawn_cube(
                world,
                prefab_cube,
                float3_zero,
                scale);
        }
        zox_set_parent(world, e2, bone_parent);*/
        if (mesh) {
            zox_add(mesh, HeldAction);
            zox_setv(mesh, LocalPosition3D, position);
            zox_setv(mesh, LocalRotation3D, rotation);
            // zox_setv(mesh, LocalScale1, scale);
            zox_add(mesh, DisableParentScale);
        }
        if (dbg_log) {
            zox_log("[%s] has Spawned Held Item [%s] at [%fx%fx%f]",
                zox_getn(e),
                zox_getn(aaction->value),
                position.x,
                position.y,
                position.z);
        }
    }
    zox_sys_on_end();
} zoxd_system(active_action_hold_system);

// the held item is a child of the hand bone, so rebuilding the body's bones (parts changing) deletes it:
//  hold it again when the hand has none (the skeleton is there and its hand bone is made)
void active_action_restore_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ActiveAction);
    zox_sys_out(ActiveActionDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ActiveAction, aaction);
        zox_sys_o(ActiveActionDirty, dirty);
        if (dirty->value || !zox_valid(aaction->value) || !zox_has(aaction->value, Item)) {
            continue;
        }
        entity skeleton = zox_get_link(world, e, SkeletonLink);
        if (!zox_valid(skeleton)) {
            continue;
        }
        entity hand = zox_get_link(world, skeleton, HandBoneLink);
        if (!zox_valid(hand) ||
            zox_valid(zox_get_child_by_id(world, hand, zox_id(HeldAction))))
        {
            continue;
        }
        dirty->value = zox_dirty_trigger;
    }
    zox_sys_on_end();
} zoxd_system(active_action_restore_system);
