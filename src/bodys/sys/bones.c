// Spawns a bone for the item part, and children
entity spawn_part_bones(ecs* world, entity skeleton, BoneLinks* bones, float3 hbounds, float bscale, entity parent, float3 parent_position, entity e) {
    if (!zox_valid(e)) {
        zox_logw("Invalid Part");
        return 0;
    }
    if (!zox_has(e, VoxelPosition)) {
        zox_logw("Invalid Part [%s]", zox_get_name(e));
        return 0;
    }
    entity model = get_item_model(world, e);
    if (!zox_valid(model) || !zox_has(model, BlockScale) || !zox_has(model, ChunkSize)) {
        zox_logw("Model components invalid for part [%s]", zox_get_name(e));
        return 0;
    }
    // zox_geter_value(model, BlockScale, float, bscale);
    zox_geter_value(e, PartPosition, byte3, pposition);
    zox_geter_value(model, ChunkSize, int3, vsize);
    float3 size = float3_scale(int3_to_float3(vsize), bscale * 0.5f);
    size = float3_add(size, float3_single(bscale * 0.5f));
    float3 position = float3_scale(byte3_to_float3(pposition), bscale);
    position = float3_sub(position, hbounds);
    position = float3_add(position, float3_single(bscale * 0.5f));
    float3 local_position = float3_sub(position, parent_position);
    // TODO: calculate local position by subtracting parent position
    entity bone = spawn_body_bone(world, prefab_bone, skeleton, parent, position, local_position, size);
    zox_set_unique_name(bone, "body_bone");
    add_to_BoneLinks(bones, bone);
    // zox_log("+ Spawned bone for Part [%s]:", zox_get_name(part));
    // zox_log("   # size  b [%ix%ix%i] f [%fx%fx%f]", vsize.x, vsize.y, vsize.z, size.x, size.y, size.z);
    // zox_log("   @ position b [%ix%ix%i] f [%fx%fx%f] l [%fx%fx%f]", pposition.x, pposition.y, pposition.z, position.x, position.y, position.z, local_position.x, local_position.y, local_position.z);
    // Now Recursively add parts
    entity parts[zox_children_capacity];
    uint parts_length = zox_get_children_by_id(world, e, parts, zox_children_capacity, zox_id(BodyPart));
    for (uint k = 0; k < parts_length; k++) {
        entity e2 = parts[k];
        entity e3 = spawn_part_bones(world, skeleton, bones, hbounds, bscale, bone, position, e2);
        if (e3) {
            zox_set_parent(world, e3, bone);
        }
    }
    return bone;
}

zox_sys2(CharacterBoneSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BodyDirty);
    zox_sys_out(BoneLinks);
    zox_sys_out(SkeletonDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BodyDirty, state);
        zox_sys_o(BoneLinks, bones);
        zox_sys_o(SkeletonDirty, dirty);
        if (state->value != zox_dirty_active) {
            continue;
        }
        // NOTE: Deletes old bones, and old parts
        for (int j = 0; j < bones->length; j++) {
            entity e2 = bones->value[j];
            zox_delete(e2);
        }
        resize_BoneLinks(bones, 0);
        entity core_part = zox_get_child_by_id(world, e, zox_id(BodyPart));
        entity core_model = get_item_model(world, core_part);
        // TODO: Body dirty should set Body's MaxRenderDepth and BlockScale, before VoxCombination
        zox_geter_value(core_model, MaxRenderDepth, byte, mdepth);
        float bscale = (1.0f / (powers_of_two_byte[mdepth]));
        zox_geter_value(e, BodySize, byte3, bsize);
        float3 hbounds = float3_scale(byte3_to_float3(bsize), bscale * 0.5f);
        entity e2 = spawn_part_bones(world, e, bones, hbounds, bscale, e, float3_zero, core_part);
        zox_set_parent(world, e2, e);
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(CharacterBoneSpawnSystem);
