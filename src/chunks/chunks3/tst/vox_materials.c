// tst: vox_materials, a vox model draws a solid mesh and a see through one (build_voxel_mesh_material): a slime's
//  solid core in a see through shell (doc/plans/chunks_refactor.md phase 3)
//  - a 16³ model: a solid ball (value 1) wrapped in a shell a voxel thick (value 2, see through: zox_block_material_liquid)
//  - the solid mesh: every face of the core, under the shell too (a see through voxel hides only its own kind's faces)
//  - the see through mesh: the shell's faces against air only (the core hides them, shell beside shell hides them)
//  - its kinds from its MaterialIDs (a material id a palette entry, as MagicaVoxel's: vox_material_table)
//  - both counted against a count of the same rule here, voxel by voxel (block_material_hides), both lookups
//    (pyramid and octree) the same
//  - run: bash bsh/tst.sh vox_materials
#define zox_tst_vm_depth 4

static inline byte zox_tst_vm_value(int x, int y, int z, int length) {
    const float c = (length - 1) * 0.5f;
    const float d = sqrtf((x - c) * (x - c) + (y - c) * (y - c) + (z - c) * (z - c));
    return d <= 4.5f ? 1 : (d <= 5.5f ? 2 : 0);
}

// faces of a kind by the rule, voxel by voxel: beside air or past the model, or a voxel that doesn't hide it
static inline int zox_tst_vm_expected(const byte* materials, byte material, int length) {
    int faces = 0;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const byte v = zox_tst_vm_value(x, y, z, length);
        if (!v || materials[v] != material) {
            continue;
        }
        for (byte d = 0; d < 6; d++) {
            const int3 n = move_position((int3) { x, y, z }, d);
            const byte inside = n.x >= 0 && n.y >= 0 && n.z >= 0 && n.x < length && n.y < length && n.z < length;
            const byte other = inside ? zox_tst_vm_value(n.x, n.y, n.z, length) : 0;
            faces += !(other && block_material_hides(materials[other], material));
        }
    }
    return faces;
}

static inline int zox_tst_vm_faces(const VoxelNode* model, const ColorRGBs* colors, const byte* materials, byte material,
    byte pyramid)
{
    mesh_colored_build_data mesh = {
        .indicies = create_int_array_d(initial_dynamic_array_size),
        .vertices = create_float3_array_d(initial_dynamic_array_size),
        .colors = create_color_array_d(initial_dynamic_array_size)
    };
    build_voxel_mesh_material(model, NULL, colors, &mesh, zox_tst_vm_depth, zox_tst_vm_depth, float3_zero, 1.0f,
        pyramid, materials, material);
    const int faces = mesh.vertices->size / 4;
    dispose_int_array_d(mesh.indicies);
    dispose_float3_array_d(mesh.vertices);
    dispose_color_array_d(mesh.colors);
    return faces;
}

byte zox_tst_vox_materials_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_vox_materials_check(ecs* world) {
    (void) world;
    const int length = octree_size(zox_tst_vm_depth);
    VoxelNode model;
    create_VoxelNode(&model);
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        set_VoxelNode(&model, zox_tst_vm_depth, (byte3) { x, y, z }, zox_tst_vm_value(x, y, z, length));
    }
    optimize_reduce_VoxelNode(&model);
    color_rgb palette[2] = { { 60, 180, 60 }, { 120, 220, 120 } };
    const ColorRGBs colors = { .value = palette, .length = 2 };
    // as a model has it, MagicaVoxel's way: a material id its palette entry, the second see through
    byte ids_value[2] = { zox_block_material_solid, zox_block_material_liquid };
    const MaterialIDs ids = { .value = ids_value, .length = 2 };
    byte materials[256];
    const byte other = vox_material_table(&ids, materials);
    zox_tst_check(other, "the kinds table found no see through material");
    const int solid_expected = zox_tst_vm_expected(materials, zox_block_material_solid, length);
    const int shell_expected = zox_tst_vm_expected(materials, zox_block_material_liquid, length);
    for (byte pyramid = 0; pyramid <= 1; pyramid++) {
        const int solid = zox_tst_vm_faces(&model, &colors, materials, zox_block_material_solid, pyramid);
        const int shell = zox_tst_vm_faces(&model, &colors, materials, zox_block_material_liquid, pyramid);
        zox_log("[tst] [metric] vox_materials %s: solid [%i] faces (expected [%i]), see through [%i] (expected [%i])",
            pyramid ? "pyramid" : "octree", solid, solid_expected, shell, shell_expected);
        zox_tst_check(solid == solid_expected, "%s: the solid mesh has [%i] faces, not [%i]", pyramid ? "pyramid" : "octree",
            solid, solid_expected);
        zox_tst_check(shell == shell_expected, "%s: the see through mesh has [%i] faces, not [%i]",
            pyramid ? "pyramid" : "octree", shell, shell_expected);
    }
    // the core shows through the shell: as many solid faces as the core alone in air would show
    zox_tst_check(solid_expected > 0 && shell_expected > 0, "no solid [%i] or see through [%i] faces", solid_expected,
        shell_expected);
    collapse_VoxelNode(&model);
}

void zox_tsts_chunks3_vox_materials() {
    zox_tst_add("vox_materials", zox_tst_vox_materials_ready, zox_tst_vox_materials_check, 30);
}
