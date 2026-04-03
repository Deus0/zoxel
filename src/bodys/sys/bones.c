// Spawn bones from body parts
// TODO: Recursively spawn child bones here for PartLinks
// TODO: Delete Old Bones

// Spawns a bone for the item part, and children
entity spawn_part_bones(ecs* world, entity skeleton, BoneLinks* bones, float3 hbounds, float bscale, entity parent, float3 parent_position, entity part) {

    if (!zox_valid(part)) {
        zox_logw("Invalid Part");
        return 0;
    }

    if (!zox_has(part, VoxelPosition) || !zox_has(part, PartLinks)) {
        zox_logw("Invalid Part [%s]", zox_get_name(part));
        return 0;
    }

    entity model = get_item_model(world, part);
    if (!zox_valid(model) || !zox_has(model, BlockScale) || !zox_has(model, ChunkSize)) {
        zox_logw("Model components invalid for part [%s]", zox_get_name(part));
        return 0;
    }

    // zox_geter_value(model, BlockScale, float, bscale);
    zox_geter_value(model, ChunkSize, int3, vsize);
    float3 size = float3_scale(int3_to_float3(vsize), bscale * 0.5f);
    size = float3_add(size, float3_single(bscale * 0.5f));

    zox_geter(part, PartLinks, parts);

    zox_geter_value(part, PartPosition, byte3, pposition);
    float3 position = float3_scale(byte3_to_float3(pposition), bscale);
    position = float3_sub(position, hbounds);
    position = float3_add(position, float3_single(bscale * 0.5f));

    float3 local_position = float3_sub(position, parent_position);

    // from bottom left
    // zox_geter_value(skeleton, BodySize, byte3, bsize);
    // float3 hbounds = float3_scale(byte3_to_float3(bsize), 0.5f * bscale);
    // zox_geter_value(skeleton, Bounds3D, float3, bounds);
    // offset by half voxel
    /*if (skeleton == parent) {
        position = float3_zero;
    }*/

    // TODO: calculate local position by subtracting parent position
    entity bone = spawn_body_bone(world, prefab_bone, skeleton, parent, position, local_position, size);
    zox_set_unique_name(bone, "body_bone");
    add_to_BoneLinks(bones, bone);

    // zox_log("+ Spawned bone for Part [%s]:", zox_get_name(part));

    // zox_log("   # size  b [%ix%ix%i] f [%fx%fx%f]", vsize.x, vsize.y, vsize.z, size.x, size.y, size.z);
    // zox_log("   @ position b [%ix%ix%i] f [%fx%fx%f] l [%fx%fx%f]", pposition.x, pposition.y, pposition.z, position.x, position.y, position.z, local_position.x, local_position.y, local_position.z);

    // Now Recursively add parts
    Children bchildren = (Children) { 0 };
    for (int i = 0; i < parts->length; i++) {
        entity sub_part = parts->value[i];

        entity e3 = spawn_part_bones(world, skeleton, bones, hbounds, bscale, bone, position, sub_part);

        if (e3) {
            add_to_Children(&bchildren, e3);
        }
    }
    zox_set_ptr(bone, Children, bchildren);

    return bone;
}

zox_sys2(CharacterBoneSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BodyDirty);
    zox_sys_in(PartLinks);
    zox_sys_out(Children);
    zox_sys_out(BoneLinks);
    zox_sys_out(SkeletonDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BodyDirty, state);
        zox_sys_i(PartLinks, parts);
        zox_sys_o(Children, children);
        zox_sys_o(BoneLinks, bones);
        zox_sys_o(SkeletonDirty, dirty);

        if (state->value != zox_dirty_active) {
            continue;
        }

        // for each body part in tree: PartLinks
        for (int j = 0; j < bones->length; j++) {
            entity e2 = bones->value[j];
            zox_delete(e2);
        }
        resize_BoneLinks(bones, 0);

        if (!parts->length) {
            continue;
        }

        entity core_model = get_item_model(world, parts->value[0]);
        // zox_geter_value_non_const(core_model, BlockScale, float, bscale);

        // TODO: Body dirty should set Body's MaxRenderDepth and BlockScale, before VoxCombination
        zox_geter_value_non_const(core_model, MaxRenderDepth, byte, mdepth);
        float bscale = (1.0f / (powers_of_two_byte[mdepth])); // 2.0f *

        // zox_geter_value(e, BlockScale, float, bscale);
        zox_geter_value(e, BodySize, byte3, bsize);
        float3 hbounds = float3_scale(byte3_to_float3(bsize), bscale * 0.5f);

        // zox_log("- Spawned Bones  vsize [%ix%ix%i] - scale [%f] - hbounds [%fx%fx%f] depth [%i]", bsize.x, bsize.y, bsize.z, bscale, hbounds.x, hbounds.y, hbounds.z, mdepth);

        // TODO: Delete old children bones, and old parts
        for (int j = 0; j < parts->length; j++) {
            entity part = parts->value[j];

            entity e2 = spawn_part_bones(world, e, bones, hbounds, bscale, e, float3_zero, part);

            add_to_Children(children, e2);
        }

        dirty->value = zox_dirty_trigger;

        // zox_log("+ Spawned Bones [%i]", bones->length);
    }
} zox_sys_end(CharacterBoneSpawnSystem);
