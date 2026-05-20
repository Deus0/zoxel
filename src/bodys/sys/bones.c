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
    if (!zox_has(part, PartPosition)) {
        zox_logw("Invalid Part [%s]", zox_get_name(part));
        return 0;
    }
    byte3 pposition = zox_getv(part, PartPosition);
    entity vox = get_item_model(world, part);
    if (!zox_valid(vox) || !zox_has(vox, ChunkSize)) {
        zox_logw("Model components invalid for part [%s]", zox_get_name(part));
        return 0;
    }
    int3 vsize = zox_getv(vox, ChunkSize);
    float3 size = float3_scale(int3_to_float3(vsize), bscale * 0.5f);
    size = float3_add(size, float3_single(bscale * 0.5f));
    float3 position = float3_scale(byte3_to_float3(pposition), bscale);
    position = float3_sub(position, half_bounds);
    position = float3_add(position, float3_single(bscale * 0.5f));
    float3 local_position = float3_sub(position, parent_position);
    // TODO: calculate local position by subtracting parent position
    entity bone = spawn_body_bone(world, prefab_bone, skeleton, parent, position, local_position, size);
    zox_set_unique_name(bone, "bbone");
    add_to_BoneLinks(bones, bone);
    if (dbg_log) {
        zox_log("+ Spawned bone for Part [%s]:", zox_get_name(part));
        zox_log("   # size  b [%ix%ix%i] f [%fx%fx%f]", vsize.x, vsize.y, vsize.z, size.x, size.y, size.z);
        zox_log("   @ position b [%ix%ix%i] f [%fx%fx%f] l [%fx%fx%f]", pposition.x, pposition.y, pposition.z, position.x, position.y, position.z, local_position.x, local_position.y, local_position.z);
    }
    // Now Recursively add parts
    entity slots[zox_children_capacity];
    uint slots_length = zox_get_children_by_id(world, slot, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < slots_length; k++) {
        entity child_slot = slots[k];
        entity child_part = zox_getv(child_slot, DataLink);
        entity child_bone = spawn_part_bones(world, skeleton, bones, half_bounds, bscale, bone, position, child_slot, child_part);
        if (child_bone) {
            zox_set_parent(world, child_bone, bone);
        }
    }
    return bone;
}

zox_sys2(CharacterBoneSpawnSystem) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BodyDirty);
    zox_sys_in(BodySize);
    zox_sys_in(BlockScale);
    zox_sys_out(BoneLinks);
    zox_sys_out(SkeletonDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BodyDirty, state);
        zox_sys_i(BodySize, bsize);
        zox_sys_i(BlockScale, bscale);
        zox_sys_o(BoneLinks, bones);
        zox_sys_o(SkeletonDirty, dirty);
        if (state->value != zox_dirty_end) { // zox_dirty_active) {
            continue;
        }
        // NOTE: Deletes old bones, and old parts
        for (int j = 0; j < bones->length; j++) {
            entity e2 = bones->value[j];
            zox_delete(e2);
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
        entity e2 = spawn_part_bones(world, e, bones, half_bounds, bscale->value, e, float3_zero, chest_slot, chest_part);
        zox_set_parent(world, e2, e);
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(CharacterBoneSpawnSystem);
