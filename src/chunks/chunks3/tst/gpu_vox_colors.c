// tst: gpu_vox_colors, the gpu mesher colours a vox model as the cpu does (doc/plans/chunks_refactor.md phase 4, 1)
//  - vox_materials' slime: a solid ball in a see through shell, each kind meshed by both (build_voxel_mesh_material,
//      gpu_mesher_set_material + gpu_mesher_set_colors)
//  - every face the same, each corner the same colour: the palette's darkened by ambient occlusion (fun/ao27.c),
//      see through voxels don't darken
//  - a corner is its face's centre and its position (a face's centre is its own in one kind's mesh), sorted and
//      compared one to one, a colour within 1 of the cpu's (float rounding)
//  - run: bash bsh/tst.sh gpu_vox_colors
byte zox_tst_gvc_lod = zox_tst_vm_depth;    // the depth meshed (lower: a lod)

typedef struct {
    int key[7];     // the face's centre doubled, its facing (its normal, 0 to 5), the corner
    uint color;
} zox_tst_gvc_corner;

static int zox_tst_gvc_compare(const void* a, const void* b) {
    return memcmp(((const zox_tst_gvc_corner*) a)->key, ((const zox_tst_gvc_corner*) b)->key, sizeof(int) * 7);
}

// a face's 4 corners as records, positions in voxels
//  - tri: its first triangle's corners, its winding (the side it's drawn from)
static inline void zox_tst_gvc_face(zox_tst_gvc_corner* out, const float3* corners, const uint* colors, const int* tri) {
    float3 centre = float3_zero;
    for (int c = 0; c < 4; c++) {
        centre = float3_add(centre, corners[c]);
    }
    // its facing: at a lod two cells can both show faces on one plane (neither's side all hides)
    const float3 n = float3_cross(float3_subtract(corners[tri[1]], corners[tri[0]]), float3_subtract(corners[tri[2]], corners[tri[0]]));
    const int axis = fabsf(n.x) > fabsf(n.y) && fabsf(n.x) > fabsf(n.z) ? 0 : (fabsf(n.y) > fabsf(n.z) ? 1 : 2);
    const int facing = axis * 2 + ((axis == 0 ? n.x : (axis == 1 ? n.y : n.z)) > 0);
    for (int c = 0; c < 4; c++) {
        out[c] = (zox_tst_gvc_corner) { { (int) roundf(centre.x / 2), (int) roundf(centre.y / 2), (int) roundf(centre.z / 2),
            facing, (int) roundf(corners[c].x), (int) roundf(corners[c].y), (int) roundf(corners[c].z) }, colors[c] };
    }
}

// the cpu's corners of one kind, faces returned
static inline int zox_tst_gvc_cpu(const VoxelNode* model, const ColorRGBs* colors, const byte* materials, byte material,
    zox_tst_gvc_corner** out)
{
    mesh_colored_build_data mesh = {
        .indicies = create_int_array_d(initial_dynamic_array_size),
        .vertices = create_float3_array_d(initial_dynamic_array_size),
        .colors = create_color_rgb_array_d(initial_dynamic_array_size)
    };
    build_voxel_mesh_material(model, NULL, colors, &mesh, zox_tst_vm_depth, zox_tst_gvc_lod, float3_zero, 1.0f, 1, 1,
        materials, material);
    const int faces = mesh.vertices->size / 4;
    *out = malloc(sizeof(zox_tst_gvc_corner) * 4 * (faces ? faces : 1));
    for (int f = 0; f < faces; f++) {
        uint face_colors[4];
        for (int c = 0; c < 4; c++) {
            const color_rgb rgb = mesh.colors->data[f * 4 + c];
            face_colors[c] = rgb.r | (rgb.g << 8) | (rgb.b << 16);
        }
        // the cpu's mesh is the model's size (1), in voxels it's the grid's
        float3 corners[4];
        for (int c = 0; c < 4; c++) {
            corners[c] = float3_scale(mesh.vertices->data[f * 4 + c], octree_size(zox_tst_vm_depth));
        }
        const int tri[3] = { mesh.indicies->data[f * 6] - f * 4, mesh.indicies->data[f * 6 + 1] - f * 4,
            mesh.indicies->data[f * 6 + 2] - f * 4 };
        zox_tst_gvc_face(*out + f * 4, corners, face_colors, tri);
    }
    dispose_int_array_d(mesh.indicies);
    dispose_float3_array_d(mesh.vertices);
    dispose_color_rgb_array_d(mesh.colors);
    return faces;
}

static uint zox_tst_gvc_room(GpuMesher* mesher, uint faces, void* data) {
    (void) data;
    return faces <= mesher->max_faces;
}

// the gpu's corners of one kind (it waits for the gpu), faces returned
static inline int zox_tst_gvc_gpu(GpuMesher* mesher, const VoxelNode* model, const ColorRGBs* colors, const byte* materials,
    byte material, zox_tst_gvc_corner** out)
{
    int3 extent[2];
    // as the game's lods (sys/vox_gpu.c): its node values and sides at the lod, the mesher already big enough
    const uint wanted = vox_gpu_mesh_grid(mesher, model, zox_tst_gvc_lod, materials, material, colors,
        zox_tst_gvc_room, NULL, extent, NULL);
    const int faces = wanted ? gpu_mesher_read_faces(mesher) : 0;
    *out = malloc(sizeof(zox_tst_gvc_corner) * 4 * (faces ? faces : 1));
    float4* vertices = malloc(sizeof(float4) * 4 * (faces ? faces : 1));
    uint* vertex_colors = malloc(sizeof(uint) * 4 * (faces ? faces : 1));
    zox_gpu_storage_read(mesher->vertices, 0, sizeof(float4) * 4 * faces, vertices);
    zox_gpu_storage_read(mesher->vertex_colors, 0, sizeof(uint) * 4 * faces, vertex_colors);
    for (int f = 0; f < faces; f++) {
        float3 corners[4];
        for (int c = 0; c < 4; c++) {
            const float4 v = vertices[f * 4 + c];
            corners[c] = float3_scale((float3) { v.x, v.y, v.z }, 1 << (zox_tst_vm_depth - zox_tst_gvc_lod));
        }
        const int tri[3] = { 0, 1, 2 };     // the shader's first triangle
        zox_tst_gvc_face(*out + f * 4, corners, vertex_colors + f * 4, tri);
    }
    free(vertices);
    free(vertex_colors);
    return faces;
}

static inline byte zox_tst_gvc_close(uint a, uint b) {
    for (int k = 0; k < 3; k++) {
        if (abs((int) ((a >> (8 * k)) & 255) - (int) ((b >> (8 * k)) & 255)) > 1) {
            return 0;
        }
    }
    return 1;
}

void zox_tst_gpu_vox_colors_check(ecs* world) {
    (void) world;
    const int length = octree_size(zox_tst_vm_depth);
    GpuMesher mesher;
    zox_tst_check(gpu_mesher_create(&mesher, length * length * length * 3), "the gpu mesher couldn't be made");
    if (!mesher.program) {
        return;
    }
    VoxelNode model;
    create_VoxelNode(&model);
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        set_VoxelNode(&model, zox_tst_vm_depth, (byte3) { x, y, z }, zox_tst_vm_value(x, y, z, length));
    }
    optimize_reduce_VoxelNode(&model);
    // as ColorRGBs and MaterialIDs: voxel v's entry v - 1
    color_rgb palette[2] = { { 60, 180, 60 }, { 200, 120, 220 } };
    const ColorRGBs colors = { .value = palette, .length = 2 };
    byte ids_value[2] = { zox_block_material_solid, zox_block_material_liquid };
    const MaterialIDs ids = { .value = ids_value, .length = 2 };
    byte materials[256];
    vox_material_table(&ids, materials);
    const byte kinds[2] = { zox_block_material_solid, zox_block_material_liquid };
    const char* names[2] = { "solid", "see through" };
    for (zox_tst_gvc_lod = zox_tst_vm_depth; zox_tst_gvc_lod >= 2; zox_tst_gvc_lod--)
    for (byte k = 0; k < 2; k++) {
        zox_tst_gvc_corner* cpu;
        zox_tst_gvc_corner* gpu;
        const int cpu_faces = zox_tst_gvc_cpu(&model, &colors, materials, kinds[k], &cpu);
        const int gpu_faces = zox_tst_gvc_gpu(&mesher, &model, &colors, materials, kinds[k], &gpu);
        zox_tst_check(cpu_faces > 0 && cpu_faces == gpu_faces, "%s: the gpu drew [%i] faces, the cpu [%i]", names[k],
            gpu_faces, cpu_faces);
        int moved = 0, recoloured = 0, darkened = 0;
        // a corner with nothing round it: the palette's times AO_MULTIPLIER
        const color_rgb c = palette[k];
        const uint open = int_min(255, (int) (c.r * AO_MULTIPLIER)) | (int_min(255, (int) (c.g * AO_MULTIPLIER)) << 8) |
            (int_min(255, (int) (c.b * AO_MULTIPLIER)) << 16);
        if (cpu_faces == gpu_faces) {
            qsort(cpu, cpu_faces * 4, sizeof(zox_tst_gvc_corner), zox_tst_gvc_compare);
            qsort(gpu, gpu_faces * 4, sizeof(zox_tst_gvc_corner), zox_tst_gvc_compare);
            for (int i = 0; i < cpu_faces * 4; i++) {
                if (zox_tst_gvc_compare(&cpu[i], &gpu[i])) {
                    if (!moved++) {
                        zox_log("[tst] %s: first corner apart at [%i]: cpu [%i %i %i | %i %i %i] gpu [%i %i %i | %i %i %i]",
                            names[k], i, cpu[i].key[0], cpu[i].key[1], cpu[i].key[2], cpu[i].key[4], cpu[i].key[5],
                            cpu[i].key[6], gpu[i].key[0], gpu[i].key[1], gpu[i].key[2], gpu[i].key[4], gpu[i].key[5],
                            gpu[i].key[6]);
                    }
                } else if (!zox_tst_gvc_close(cpu[i].color, gpu[i].color)) {
                    if (!recoloured++) {
                        zox_log("[tst] %s: first colour apart at [%i]: cpu [%06x] gpu [%06x]", names[k], i, cpu[i].color,
                            gpu[i].color);
                    }
                }
                darkened += !zox_tst_gvc_close(cpu[i].color, open);
            }
        }
        zox_log("[tst] [metric] gpu_vox_colors depth %i %s: [%i] faces, [%i] corners darkened", zox_tst_gvc_lod, names[k], cpu_faces, darkened);
        zox_tst_check(!moved, "%s: [%i] corners weren't where the cpu's are", names[k], moved);
        zox_tst_check(!recoloured, "%s: [%i] corners weren't the cpu's colour", names[k], recoloured);
        zox_tst_check(darkened > 0, "%s: no corner was darkened by ambient occlusion", names[k]);
        free(cpu);
        free(gpu);
    }
    collapse_VoxelNode(&model);
    gpu_mesher_dispose(&mesher);
}

void zox_tsts_chunks3_gpu_vox_colors() {
    zox_tst_add("gpu_vox_colors", zox_tst_gpu_mesh_ready, zox_tst_gpu_vox_colors_check, 60);
}
