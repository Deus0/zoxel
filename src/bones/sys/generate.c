// todo: Shapes on bones used for setting verts, spheres of influence
// NOTE: If Skeleton Dirty, or MeshDirty, Generate Weights

// each vertex moves with the bone it's deepest in: its distance from the bone's middle over its half size, the
//  largest of the three axes (under 1 inside its box)
//  - a chest's rib stays with the chest, not a shoulder whose box reaches over it (it came away when the arms swung)
//  - a vertex outside every box goes with the box nearest it (an arm's front poking past its box stayed with the chest,
//    the sleeve tore and the skin showed through when it swung)
//  - a score per bone, lowest wins: inside its box its depth less 1 (under 0), outside how far it is from the box
static inline float bone_vertex_score(float3 p, float3 c, float3 s) {
    const float3 d = { fabsf(p.x - c.x), fabsf(p.y - c.y), fabsf(p.z - c.z) };
    const float depth = fmaxf(d.x / s.x, fmaxf(d.y / s.y, d.z / s.z));
    if (depth <= 1) {
        return depth - 1;
    }
    const float3 out = { fmaxf(d.x - s.x, 0), fmaxf(d.y - s.y, 0), fmaxf(d.z - s.z, 0) };
    return sqrtf(out.x * out.x + out.y * out.y + out.z * out.z);
}

void generate_weights(
    ecs* world,
    entity e,
    const MeshVertices* verts,
    const BoneLinks* bones,
    BoneIndexes* weights)
{
    float3 bone_positions[bones->length];
    float3 bone_sizes[bones->length];
    for (int j = 0; j < bones->length; j++) {
        bone_positions[j] = zox_getv(bones->value[j], BonePosition);
        bone_sizes[j] = zox_getv(bones->value[j], BoneSize);
    }
    resize_BoneIndexes(weights, verts->length);
    for (int j = 0; j < verts->length; j++) {
        const float3 p = verts->value[j];
        weights->value[j] = 0;
        if (disable_bones) {
            continue;
        }
        float best = 1000000;
        for (int k = bones->length - 1; k >= 0; k--) {
            const float score = bone_vertex_score(p, bone_positions[k], bone_sizes[k]);
            if (score < best) {
                best = score;
                weights->value[j] = k;
            }
        }
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
        const BoneLinks* bones = zox_get(skeleton, BoneLinks);
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
