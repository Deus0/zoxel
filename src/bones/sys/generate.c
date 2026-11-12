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
        if (block_depth >= 5) { // && zox_has(bone, ArmBone)) {
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

// When the skeleton updates, regenerate weights for all its meshes
void bone_index_generate_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity mesh = it2.entities[j];
                if (!zox_has(mesh, SkeletonMesh) ||
                    zox_has(mesh, BuildMesh) ||
                    zox_has(mesh, BuildMeshWeights))
                {
                    continue;
                }
                zox_add(mesh, BuildMeshWeights);
            }
        }
        if (dbg_log) {
            zox_log("Skeleton Updated [%s], rebuilding weights", zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(bone_index_generate_system);

// Generates a skeleton mesh's weights from its parent's bones
void bone_index_generate_system2(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshVertices);
    zox_sys_out(BoneIndexes);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshVertices, verts);
        zox_sys_o(BoneIndexes, weights);
        zox_remove(e, BuildMeshWeights);
        entity skeleton = zox_get_parent(world, e);
        if (!zox_valid(skeleton) || !zox_has(skeleton, BoneLinks)) {
            zox_loge("SkeletonMesh [%s] has no parent skeleton", zox_sys_e_name);
            continue;
        }
        zox_geter(skeleton, BoneLinks, bones);
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
    }
    zox_sys_on_end();
} zoxd_system(bone_index_generate_system2);
