// todo: Shapes on bones used for setting verts, spheres of influence
// todo: use SphereRadius float per bone for a simple radius
zox_sys2(BoneIndexGenerateSystem) {
    byte dbg_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkeletonDirty);
    zox_sys_in(MeshDirty);
    zox_sys_in(MeshVertices);
    zox_sys_in(BoneLinks);
    zox_sys_out(BoneIndexes)
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkeletonDirty, skeleton_dirty);
        zox_sys_i(MeshDirty, mesh_dirty);
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(BoneLinks, bones);
        zox_sys_o(BoneIndexes, weights);
        if (!(skeleton_dirty->value == zox_dirty_active || mesh_dirty->value == mesh_state_skeleton_generate)) {
            continue;
        }
        if (dbg_log) {
            zox_log("Skeleton [%s] Updating Weights with [%i] Bones.", zox_get_name(e), bones->length);
        }
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
            bones_bounds_lower[j] = float3_subtract(bone_positions[j], half_size);
            bones_bounds_upper[j] = float3_add(bone_positions[j], half_size);
        }
        resize_BoneIndexes(weights, verts->length);
        for (int j = 0; j < verts->length; j++) {
            float3 position = verts->value[j];
            // default weight
            weights->value[j] = 0;
            for (int k = bones->length - 1; k >= 0; k--) {
                float3 lower = bones_bounds_lower[k];
                float3 upper = bones_bounds_upper[k];
                // check in constraints
                if (position.x >= lower.x && position.x <= upper.x && position.y >= lower.y && position.y <= upper.y && position.z >= lower.z && position.z <= upper.z) {
                    weights->value[j] = k;
                    /*if (k == 1) {
                        zox_log("Bone Vert Index Found: %i (vert %i)\n", k, j)
                    }*/
                    break;
                }
                /*if (j >= verts->length - 100) {
                    zox_log("Bone Vert Index Found: %i (vert %i)\n", k, j)
                    zox_log("  > position: %fx%fx%f", position.x, position.y, position.z)
                    zox_log("  > lower: %fx%fx%f", lower.x, lower.y, lower.z)
                    zox_log("  > upper: %fx%fx%f", upper.x, upper.y, upper.z)
                }*/
            }
            // for each position, check all bones, and see if it is within them
            //if (position.y >= 0.12f * 0.5f) weights->value[j] = 1;
            //else weights->value[j] = 0;
        }
    }
} zox_sys_end(BoneIndexGenerateSystem);
