// tst: vox_see_through, a model with a see through material draws two meshes a lod: its solid voxels' and, beside it,
//  its see through voxels' (a TransparentMesh sibling, set_vox_lods_materials): a slime's core in its shell
//  - a vox_lods model (spawn_vox_lods), 16³: a solid ball (palette entry 1) in a shell a voxel thick (entry 2), its
//    material ids (MaterialIDs, as MagicaVoxel's) solid and see through
//  - once both full detail meshes are built: the solid one the core's faces (through the shell too), the see through
//    one the shell's against air, the counts tst vox_materials checks at the mesher (360, 528)
//  - run: bash bsh/tst.sh vox_see_through
entity zox_tst_vst_model = 0;
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

byte zox_tst_vox_see_through_ready(ecs* world) {
    zox_tst_vst_frames++;
    if (!zox_tst_vst_model) {
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
        colors.value[0] = (color_rgb) { 60, 180, 60 };
        colors.value[1] = (color_rgb) { 120, 220, 120 };
        zox_set_ptr(zox_tst_vst_model, ColorRGBs, colors);
        dispose_ColorRGBs(&colors);
        const byte ids[2] = { zox_block_material_solid, zox_block_material_liquid };
        set_vox_lods_materials(world, zox_tst_vst_model, ids, 2);
        return 0;
    }
    zox_tst_waiting = "the model's solid and see through meshes built";
    const entity solid = zox_tst_vst_mesh(world, 0);
    const entity see_through = zox_tst_vst_mesh(world, 1);
    return zox_tst_vst_frames > 3 && solid && see_through && !zox_has(solid, BuildMesh) && !zox_has(see_through, BuildMesh);
}

void zox_tst_vox_see_through_check(ecs* world) {
    const entity solid = zox_tst_vst_mesh(world, 0);
    const entity see_through = zox_tst_vst_mesh(world, 1);
    zox_tst_check(solid && see_through, "the model has no solid [%i] or see through [%i] mesh", solid != 0, see_through != 0);
    if (!solid || !see_through) {
        return;
    }
    const int solid_faces = zox_get(solid, MeshVertices)->length / 4;
    const int see_through_faces = zox_get(see_through, MeshVertices)->length / 4;
    zox_log("[tst] [metric] vox_see_through: solid mesh [%i] faces, see through [%i] (TransparentMesh [%i])", solid_faces,
        see_through_faces, zox_has(see_through, TransparentMesh));
    zox_tst_check(solid_faces == 360, "the solid mesh has [%i] faces, not [360]", solid_faces);
    zox_tst_check(see_through_faces == 528, "the see through mesh has [%i] faces, not [528]", see_through_faces);
}
