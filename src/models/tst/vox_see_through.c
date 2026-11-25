// tst: vox_see_through, a model with a see through material draws two meshes a lod: its solid voxels' and, beside it,
//  its see through voxels' (a TransparentMesh sibling, set_vox_lods_materials): a slime's core in its shell
//  - a vox_lods model (spawn_vox_lods), 16³: a solid ball (palette entry 1) in a shell a voxel thick (entry 2), its
//    material ids (MaterialIDs, as MagicaVoxel's) solid and see through
//  - once both full detail meshes are built: the solid one the core's faces (through the shell too), the see through
//    one the shell's against air, the counts tst vox_materials checks at the mesher (360, 528)
//  - run: bash bsh/tst.sh vox_see_through
entity zox_tst_vst_model = 0;
entity zox_tst_vst_material = 0;
int zox_tst_vst_frames = 0;

// a model's full detail mesh of a kind (see through or not), 0 if none yet
static inline entity zox_tst_vst_mesh(ecs* world, byte see_through) {
    const byte depth = zox_getv(zox_tst_vst_model, NodeDepth);
    entity found = 0;
    iter it = zox_children(world, zox_tst_vst_model);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity child = it.entities[i];
            if (zox_has(child, VoxLodMesh) && zox_getv(child, RenderDepth) == depth &&
                zox_has(child, TransparentMesh) == see_through)
            {
                found = child;
            }
        }
    }
    return found;
}

// the slime shaped model: a solid ball in a see through shell, its gel a material at alpha 0.5
static inline void zox_tst_vst_spawn(ecs* world) {
    const byte depth = 4;
    const int length = octree_size(depth);
    zox_tst_vst_model = spawn_vox_lods(world, 0, "see_through", depth, NULL);
    VoxelNode* voxels = zox_get_mut(zox_tst_vst_model, VoxelNode);
    const float c = (length - 1) * 0.5f;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const float d = sqrtf((x - c) * (x - c) + (y - c) * (y - c) + (z - c) * (z - c));
        set_VoxelNode(voxels, depth, (byte3) { x, y, z }, d <= 4.5f ? 1 : (d <= 5.5f ? 2 : 0));
    }
    optimize_reduce_VoxelNode(voxels);
    ColorRGBs colors = { 0 };
    resize_ColorRGBs(&colors, 2);
    colors.value[0] = (color_rgb) { 200, 60, 60 };
    colors.value[1] = (color_rgb) { 90, 220, 110 };
    zox_set_ptr(zox_tst_vst_model, ColorRGBs, colors);
    dispose_ColorRGBs(&colors);
    zox_tst_vst_material = spawn_vox_material_see_through(world, 0.5f);
    const byte ids[2] = { zox_block_material_solid, zox_block_material_liquid };
    set_vox_lods_materials(world, zox_tst_vst_model, ids, 2, zox_tst_vst_material);
}

byte zox_tst_vox_see_through_ready(ecs* world) {
    zox_tst_vst_frames++;
    if (!zox_tst_vst_model) {
        zox_tst_vst_spawn(world);
        return 0;
    }
    zox_tst_waiting = "the model's solid and see through meshes built";
    const entity solid = zox_tst_vst_mesh(world, 0);
    const entity see_through = zox_tst_vst_mesh(world, 1);
    // built, and a gpu mesh's merged count back (greedy)
    return zox_tst_vst_frames > 3 && solid && see_through && !zox_has(solid, BuildMesh) && !zox_has(see_through, BuildMesh) &&
        !vox_gpu_pending(world, solid) && !vox_gpu_pending(world, see_through);
}

void zox_tst_vox_see_through_check(ecs* world) {
    const entity solid = zox_tst_vst_mesh(world, 0);
    const entity see_through = zox_tst_vst_mesh(world, 1);
    zox_tst_check(solid && see_through, "the model has no solid [%i] or see through [%i] mesh", solid != 0, see_through != 0);
    if (!solid || !see_through) {
        return;
    }
    // as voxel faces: the same however its faces merge (greedy meshing)
    const float voxel = zox_getv(zox_tst_vst_model, BlockScale);
    const int solid_faces = mesh3_voxel_faces(world, solid, voxel);
    const int see_through_faces = mesh3_voxel_faces(world, see_through, voxel);
    zox_log("[tst] [metric] vox_see_through: solid mesh [%i] faces, see through [%i] (TransparentMesh [%i])", solid_faces,
        see_through_faces, zox_has(see_through, TransparentMesh));
    zox_log("[tst] [metric] vox_see_through: drawn as [%i] and [%i] quads (greedy meshing [%i])",
        mesh_index_count(world, solid) / 6, mesh_index_count(world, see_through) / 6, zox_greedy_mesh);
    zox_tst_check(solid_faces == 360, "the solid mesh has [%i] faces, not [360]", solid_faces);
    zox_tst_check(see_through_faces == 528, "the see through mesh has [%i] faces, not [528]", see_through_faces);
    zox_tst_check(zox_get_link(world, see_through, MaterialLink) == zox_tst_vst_material,
        "the see through mesh doesn't link its material (MaterialLink)");
    zox_tst_check(instance_see_through_alpha(world, see_through) == 0.5f, "the see through mesh draws at alpha [%.2f], not "
        "its material's [0.5]", instance_see_through_alpha(world, see_through));
}

// tst: slime_view, the slime shaped model drawn instanced (instance_views' 4 views, its own model): its red core
//  through its green see through shell, blended (vox_instance_render_system's see through pass)
//  - look at tst/slime_view/slime_view.ppm; checks as instance_views: each view drew it
//  - run: bash bsh/tst.sh slime_view
byte zox_tst_slime_view_ready(ecs* world) {
    if (!zox_tst_vst_model && zox_valid(local_game)) {
        zox_tst_vst_spawn(world);
        zox_tst_model_views_model = zox_tst_vst_model;
    }
    zox_tst_model_views_instanced = 1;
    zox_tst_model_views_empty_file = "slime_view_empty.ppm";
    zox_tst_model_views_file = "slime_view.ppm";
    return zox_tst_model_views_ready(world);
}
