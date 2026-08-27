// todo: Shapes on bones used for setting verts, spheres of influence
// todo: use SphereRadius float per bone for a simple radius
// NOTE: If Skeleton Dirty, or MeshDirty, Generate Weights

void generate_weights(
    ecs* world,
    entity e,
    const MeshVertices* verts,
    const BoneLinks* bones,
    BoneIndexes* weights)
{
    // get all children bone positions and sizes
    float3 bone_positions[bones->length];
    float3 bone_sizes[bones->length];
    float3 bones_bounds_lower[bones->length];
    float3 bones_bounds_upper[bones->length];
    for (int j = 0; j < bones->length; j++) {
        entity bone = bones->value[j];
        bone_positions[j] = zox_getv(bone, BonePosition);
        bone_sizes[j] = zox_getv(bone, BoneSize);
        float3 half_size = bone_sizes[j];
        if (block_vox_depth >= 5) { // && zox_has(bone, ArmBone)) {
            half_size = float3_multiply1(half_size, 1.2f);
        }
        bones_bounds_lower[j] = float3_subtract(
            bone_positions[j],
            half_size);
        bones_bounds_upper[j] = float3_add(
            bone_positions[j],
            half_size);
    }
    resize_BoneIndexes(weights, verts->length);
    for (int j = 0; j < verts->length; j++) {
        float3 position = verts->value[j];
        // default weight
        weights->value[j] = 0;
        if (disable_bones) {
            continue;
        }
        for (int k = bones->length - 1; k >= 0; k--) {
            float3 lower = bones_bounds_lower[k];
            float3 upper = bones_bounds_upper[k];
            // check in constraints
            if (position.x >= lower.x &&
                position.x <= upper.x &&
                position.y >= lower.y &&
                position.y <= upper.y &&
                position.z >= lower.z &&
                position.z <= upper.z)
            {
                weights->value[j] = k;
                /*if (k == 1) {
                 z ox_log("Bone *Vert Index Found: %i (vert %i)\n", k, j)
            }*/
                break;
            }
            /*if (j >= verts->length - 100) {
             z ox_log("Bone Vert* Index Found: %i (vert %i)\n", k, j)
             zox_log("  > position: %fx%fx%f", position.x, position.y, position.z)
             zox_log("  > lower: %fx%fx%f", lower.x, lower.y, lower.z)
             zox_log("  > upper: %fx%fx%f", upper.x, upper.y, upper.z)
        }*/
        }
        // for each position, check all bones, and see if it is within them
        //if (position.y >= 0.12f * 0.5f) weights->value[j] = 1;
        //else weights->value[j] = 0;
    }
    zox_add(e, SkeletonMeshDirty);
}

zox_sys2(BoneIndexGenerateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkeletonDirty);
    zox_sys_in(MeshVertices);
    zox_sys_in(BoneLinks);
    zox_sys_out(BoneIndexes);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkeletonDirty, skeleton_dirty);
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(BoneLinks, bones);
        zox_sys_o(BoneIndexes, weights);
        if (skeleton_dirty->value != zox_dirty_active) {
            // mesh_dirty->value == mesh_state_skeleton_generate)) {
            continue;
        }
        generate_weights(
            world,
            e,
            verts,
            bones,
            weights);
        if (dbg_log) {
            zox_log("Weights Updated [%s] Bones [%i]",
                zox_sys_e_name,
                bones->length);
        }
        // mesh_dirty->value = paint_bone_weights ? mesh_state_skeleton_paint : mesh_state_skeleton_end;
    }
} zox_sys_end(BoneIndexGenerateSystem);


zox_sys2(BoneIndexGenerateSystem2) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshVertices);
    zox_sys_in(BoneLinks);
    zox_sys_out(BoneIndexes);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(BoneLinks, bones);
        zox_sys_o(BoneIndexes, weights);
        generate_weights(
            world,
            e,
            verts,
            bones,
            weights);
        zox_remove(e, BuildMeshWeights);
        if (dbg_log) {
            zox_log("Weights Updated 2 [%s] Bones [%i]",
                zox_sys_e_name,
                bones->length);
        }
    }
} zox_sys_end(BoneIndexGenerateSystem2);

