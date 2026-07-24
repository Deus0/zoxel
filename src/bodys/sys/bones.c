// Spawns a bone for the item part, and children
entity spawn_part_bones(ecs* world, entity skeleton, BoneLinks* bones, float3 half_bounds, float bscale, entity parent, float3 parent_position, entity slot, entity part) {
    byte dbg_log = 0;
    if (!zox_valid(slot)) {
        zox_logw("Invalid Slot for Bones");
        return 0;
    }
    // Many slots on bodys can have no parts
    if (!zox_valid(part)) {
        // zox_logw("Invalid Part for Slot [%s]", zox_get_name(slot));
        return 0;
    }
    if (!zox_has(slot, PartPosition)) {
        zox_logw("Invalid Part [%s]", zox_get_name(part));
        return 0;
    }
    byte3 part_position = zox_getv(slot, PartPosition);
    byte3 part_size = zox_getv(slot, PartSize);
    byte3 part_centre_position = byte3_add(part_position, byte3_half(part_size));
    entity vox = get_item_model(world, part);
    if (!zox_valid(vox) || !zox_has(vox, ChunkSize)) {
        zox_logw("Model components invalid for part [%s]", zox_get_name(part));
        return 0;
    }
    int3 vsize = zox_getv(vox, ChunkSize);
    float3 size = float3_scale(int3_to_float3(vsize), bscale * 0.5f);
    size = float3_add(size, float3_single(bscale * 0.5f));
    float3 position = float3_scale(byte3_to_float3(part_centre_position), bscale);
    position = float3_sub(position, half_bounds);
    position = float3_add(position, float3_single(bscale * 0.5f));
    float3 local_position = float3_sub(position, parent_position);
    // TODO: calculate local position by subtracting parent position
    entity bone = spawn_body_bone(world, prefab_bone, skeleton, parent, position, local_position, size);
    zox_set_unique_name(bone, "bbone");
    add_to_BoneLinks(bones, bone);
    // TODO: FLip models around
    byte is_right_side = position.x < 0;
    if (zox_has(part, Head)) {
        zox_add_tag(bone, HeadBone);
        zox_set(skeleton, HeadBoneLink, { bone });
    }
    if (zox_has(part, ArmPart)) {
        zox_add_tag(bone, ArmBone);
    }
    if (zox_has(part, Hips)) {
        zox_add_tag(bone, HipsBone);
    }
    if (zox_has(part, Shoulder)) {
        zox_add_tag(bone, ShoulderBone);
        zox_set(bone, SwingState, { 1 });
        zox_set(bone, SwingAngle, { 110 });
        // NOTE: For Right Shoulder
        if (is_right_side) {
            zox_set(skeleton, ShoulderBoneLink, { bone });
        }
    }
    if (zox_has(part, Hand)) {
        zox_add_tag(bone, HandBone);
        // NOTE: For Right Hand
        if (is_right_side) {
            zox_set(skeleton, HandBoneLink, { bone });
            // zox_log("Added RightHandBone to Part's Bone [%s]", zox_get_name(part));
        }
    }
    if (zox_has(part, Thigh)) {
        zox_add_tag(bone, ThighBone);
        zox_set(bone, SwingState, { 1 });
        zox_set(bone, SwingAngle, { 60 });
        // zox_log("Added ThighBone to Part's Bone [%s]", zox_get_name(part));
    }
    if (dbg_log) {
        zox_log("Spawned Part [%s] Bone:", zox_get_name(part));
        zox_log("   # size  b [%ix%ix%i] f [%fx%fx%f]", vsize.x, vsize.y, vsize.z, size.x, size.y, size.z);
        zox_log("   @ position b [%ix%ix%i] f [%fx%fx%f] l [%fx%fx%f]", part_centre_position.x, part_centre_position.y, part_centre_position.z, position.x, position.y, position.z, local_position.x, local_position.y, local_position.z);
    }
    // Now Recursively add parts
    iter it2 = zox_children(world, slot);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity e2 = it2.entities[j];
            if (!zox_valid(e2) || !zox_has(e2, Slot)) {
                continue;
            }
            entity child_part = zox_getv(e2, DataLink);
            entity child_bone = spawn_part_bones(world, skeleton, bones, half_bounds, bscale, bone, position, e2, child_part);
            if (child_bone) {
                zox_set_parent(world, child_bone, bone);
            }
        }
    }
    return bone;
}

// NOTE: We could delete only bones that are linked to updated parts here, but that is a refactor, perhaps later
zox_sys2(CharacterBoneSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BodySize);
    zox_sys_in(BlockScale);
    zox_sys_out(BodyDirty);
    zox_sys_out(BoneLinks);
    zox_sys_out(SkeletonDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BodySize, bsize);
        zox_sys_i(BlockScale, bscale);
        zox_sys_o(BodyDirty, body_dirty);
        zox_sys_o(BoneLinks, bones);
        zox_sys_o(SkeletonDirty, dirty);
        if (body_dirty->value != zox_generate_body_bones) {
            continue;
        }
        // NOTE: Deletes old bones, and old parts
        entity camera = zox_get_child_by_id_recursive(world, e, zox_id(Camera));
        if (zox_valid(camera)) {
            zox_set_parent(world, camera, 0);
        }
        entity root_bone = zox_get_child_by_id(world, e, zox_id(Bone));
        if (zox_valid(root_bone)) {
            zox_delete(root_bone);
        }
        resize_BoneLinks(bones, 0);
        float3 half_bounds = float3_scale(byte3_to_float3(bsize->value), bscale->value * 0.5f);
        if (dbg_log) {
            zox_log("+ [%s] is Spawning Bones", zox_get_name(e));
            zox_log("   - Size [%ix%ix%i]", bsize->value.x, bsize->value.y, bsize->value.z);
            zox_log("   - Bounds [%fx%fx%f]", half_bounds.x, half_bounds.y, half_bounds.z);
        }
        entity chest_slot = zox_get_child_by_id(world, e, zox_id(Body));
        if (!zox_valid(chest_slot)) {
            continue;
        }
        entity chest_part = zox_getv(chest_slot, DataLink);
        spawn_part_bones(world, e, bones, half_bounds, bscale->value, e, float3_zero, chest_slot, chest_part);
        dirty->value = zox_dirty_trigger;
        body_dirty->value = zox_generate_body_end;
    }
} zox_sys_end(CharacterBoneSpawnSystem);
