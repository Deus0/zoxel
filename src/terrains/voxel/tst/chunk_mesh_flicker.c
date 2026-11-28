// tst: chunk_mesh_flicker, walking through the terrain no chunk mesh flickers, each frame as it was drawn
//  - a world of its own, a streamer moving smoothly (a speed in chunks a second, not a step a frame) along x, z,
//    back, so chunks change lod as it goes: every frame from the start, after the frame's draw:
//      - blank: a chunk's material drew faces, then none, then faces again with some inside the chunk (frames of sky
//        where land was); one that settles with none was covered, its border's faces are the seam check's
//      - partial: a shown gpu mesh drawn while its faces didn't fit its room (it's meshed again bigger after, faces
//        missing until then), read back from its command and count buffers
//      - seams: a hole through a border between two chunks as drawn this frame (seams.c, every face read back), for
//        the chunks whose drawing changed this frame (a seam starts when one side changes), both sides ready
//      - light dips: a material's light as drawn (its faces' lights read back: gpu its vertex lights, cpu its colours)
//        falls by a quarter or more and comes back within 15 frames (logged: geometry moves it too), and light steps: its lod swapped
//        and the new mesh's upward faces' light differs from the old's by a fifth and an eighth of full light (in the
//        dark, 16 to 45, it doesn't show), logged; it fails when one side's drawn dark (2x darklight at most) and the
//        other 3x that, its chunk's lights all dark under sunlit air: never lit (cleared for a finer lod, nothing lit
//        it again); coarse and fine drawing apart otherwise is their shapes (a coarse cell, one light); a
//        liquid's dips aren't counted (its surface changes as it flows)
//  - lod_swap checks a chunk's first mesh alone; this checks each material and what the gpu really drew
//  - run: bash bsh/tst.sh chunk_mesh_flicker --vulkan (gpu meshing; --cpu_meshing for the cpu's), medium terrain;
//    as a player flies: zox_tst_flicker_speed=3 bash bsh/tst.sh chunk_mesh_flicker --vulkan --far 16: far chunks
//    generated coarse grow finer as it comes (their voxels again, their lights cleared): at medium every chunk's
//    generated fine from the start and that never happens (its dark flashes weren't seen there)
#define zox_tst_cmf_slots 32768     // a power of two, chunks and their materials
float zox_tst_cmf_speed = 1.5f;     // chunks a second (zox_tst_flicker_speed)
byte zox_tst_cmf_lights_only = 0;   // lod_light_flash: its lights alone, no seam checks (they're slow, and fail apart)
#define zox_tst_cmf_logs 40

typedef struct {
    entity key;         // a chunk material (faces), or a chunk (its drawing's signature)
    uint value;
} zox_tst_cmf_slot;

static const float3 zox_tst_cmf_path[] = { { 6, 0, 0 }, { 0, 0, 6 }, { -6, 0, 0 }, { 0, 0, -6 } };
#define zox_tst_cmf_legs ((int) (sizeof(zox_tst_cmf_path) / sizeof(zox_tst_cmf_path[0])))

zox_tst_cmf_slot zox_tst_cmf_maps[2][zox_tst_cmf_slots];
zox_tst_cmf_slot zox_tst_cmf_pending[zox_tst_cmf_slots];
zox_tst_cmf_slot zox_tst_cmf_changed_at[zox_tst_cmf_slots];
// a material's light as drawn, last frame
typedef struct {
    entity key;         // a chunk material
    float light;        // its shown meshes' mean light, 0 to 255 (-1 none drawn)
    entity mesh;        // its first shown mesh
    float dip_from;     // its light before it dipped, 0 if it isn't dipped
    int dip_frame;
} zox_tst_cmf_light;
zox_tst_cmf_light zox_tst_cmf_lights[zox_tst_cmf_slots];
int zox_tst_cmf_dips = 0, zox_tst_cmf_steps = 0, zox_tst_cmf_swaps_lit = 0, zox_tst_cmf_dark_steps = 0;

static inline zox_tst_cmf_light* zox_tst_cmf_light_find(entity key) {
    uint index = (uint) ((key * 0x9e3779b97f4a7c15ULL) >> 49) & (zox_tst_cmf_slots - 1);
    while (zox_tst_cmf_lights[index].key && zox_tst_cmf_lights[index].key != key) {
        index = (index + 1) & (zox_tst_cmf_slots - 1);
    }
    zox_tst_cmf_lights[index].key = key;
    return &zox_tst_cmf_lights[index];
}

// each face's drawn light by its mesh, corner and direction, while walking: a face darker by over 48 that comes back
//  within 30 frames (a flash), or one that showed dark and brightens by over 48 within 30 frames of first showing (drawn
//  before it was lit): what's seen as dark flickering flying about, a few faces at a time, under an average's notice
#define zox_tst_cmf_face_slots (1 << 21)
#define zox_tst_cmf_face_change 48
#define zox_tst_cmf_face_frames 30
typedef struct {
    uint64_t key;       // (not ulong: 32 bits on windows, the corner's bits shifted out, faces shared keys)
    byte light;
    byte dropped_from;      // its light before it darkened, 0 if it hasn't
    int frame;              // when it darkened, or first showed
} zox_tst_cmf_face;
zox_tst_cmf_face* zox_tst_cmf_faces = NULL;
int zox_tst_cmf_face_flashes = 0, zox_tst_cmf_face_late = 0, zox_tst_cmf_faces_seen = 0, zox_tst_cmf_face_reads = 0, zox_tst_cmf_mesh_reads = 0;
entity zox_tst_cmf_flash_chunks[8];
int zox_tst_cmf_flash_chunks_count = 0;
extern byte zox_tst_cmf_stage;
extern int zox_tst_cmf_frames;

static inline void zox_tst_cmf_face_light(ecs* world, entity mesh, entity chunk, float4 corner, byte direction, byte light) {
    zox_tst_cmf_face_reads++;
    if (zox_tst_cmf_stage != 2) {
        return;
    }
    if (!zox_tst_cmf_faces) {
        zox_tst_cmf_faces = calloc(zox_tst_cmf_face_slots, sizeof(zox_tst_cmf_face));
    }
    // its mesh, corner and direction mixed (splitmix64): unmixed, a mesh's faces shared a few slots, most were dropped
    uint64_t key = (uint64_t) mesh ^ ((uint64_t) ((int) (corner.x * 2) & 0xFFF) << 32) ^
        ((uint64_t) ((int) (corner.y * 2) & 0xFFF) << 44) ^ ((uint64_t) ((int) (corner.z * 2) & 0xFF) << 56) ^
        ((uint64_t) direction << 29);
    key += 0x9e3779b97f4a7c15ULL;
    key = (key ^ (key >> 30)) * 0xbf58476d1ce4e5b9ULL;
    key = (key ^ (key >> 27)) * 0x94d049bb133111ebULL;
    key = (key ^ (key >> 31)) | 1;
    uint index = (uint) key & (zox_tst_cmf_face_slots - 1);
    for (int probe = 0; probe < 64; probe++) {
        zox_tst_cmf_face* face = &zox_tst_cmf_faces[index];
        if (!face->key) {
            *face = (zox_tst_cmf_face) { key, light, 0, zox_tst_cmf_frames };
            zox_tst_cmf_faces_seen++;
            return;
        }
        if (face->key == key) {
            const int since = zox_tst_cmf_frames - face->frame;
            if (face->dropped_from) {
                if (light + 16 >= face->dropped_from) {
                    if (since <= zox_tst_cmf_face_frames) {
                        zox_tst_cmf_face_flashes++;
                        static int logged_flashes = 0;
                        if (logged_flashes++ < 12) {
                            zox_log("[tst]  face flash: mesh [%lu] dir [%i] at [%.1f %.1f %.1f] light [%i] to [%i] to [%i] over [%i] frames",
                                (ulong) mesh, direction, corner.x, corner.y, corner.z,
                                face->dropped_from, face->light, light, since);
                        }
                        if (zox_tst_cmf_flash_chunks_count < 8) {
                            zox_tst_cmf_flash_chunks[zox_tst_cmf_flash_chunks_count++] = chunk;
                        }
                    }
                    face->dropped_from = 0;
                }
            } else if (light + zox_tst_cmf_face_change < face->light) {
                face->dropped_from = face->light;
                face->frame = zox_tst_cmf_frames;
            } else if (light > face->light + zox_tst_cmf_face_change && since <= zox_tst_cmf_face_frames) {
                // drawn dark and lit within frames: a flash (light arriving later, a slow rise, isn't one)
                zox_tst_cmf_face_late++;
                static int logged_late = 0;
                if (logged_late++ < 10) {
                    const int3 at = zox_getv(chunk, ChunkPosition);
                    const entity tunk = zox_get_link(world, chunk, TunkLink);
                    zox_log("[tst]  face dark then lit: chunk [%ix%ix%i] distance [%i] unloading [%i] mesh [%lu] dir [%i] at [%.1f %.1f %.1f] light [%i] to [%i] after [%i] frames",
                        at.x, at.y, at.z, zox_getv(chunk, RenderDistance), zox_valid(tunk) && zox_has(tunk, TunkUnloading),
                        (ulong) mesh, direction, corner.x, corner.y, corner.z, face->light, light, since);
                }
                if (zox_tst_cmf_flash_chunks_count < 8) {
                    zox_tst_cmf_flash_chunks[zox_tst_cmf_flash_chunks_count++] = chunk;
                }
            }
            face->light = light;
            return;
        }
        index = (index + 1) & (zox_tst_cmf_face_slots - 1);
    }
}

// a shown mesh's light summed over what it draws, its upward faces' apart (the same surface, open to the sky, at any
//  lod: comparing all faces, a fine mesh has more in shade than a coarse one); gpu only for the upward faces (its
//  vertices' directions read back), cpu its colours
static inline void zox_tst_cmf_mesh_light(ecs* world, entity mesh, entity chunk, uint faces, double* sum, uint* count, double* up_sum, uint* up_count) {
    if (!faces) {
        return;
    }
    zox_tst_cmf_mesh_reads++;
    if (zox_has(mesh, GpuMesh)) {
        // two uints a vertex: its face's 4 corner lights, the sky's then blocks' (a terrain mesh's light_stride); a
        //  vec4 a vertex, w its direction (bits 8 to 10)
        const uint vertices = faces * 4;
        uint* lights = malloc(vertices * 2 * sizeof(uint));
        float4* positions = malloc(vertices * sizeof(float4));
        if (lights && positions && zox_gpu_storage_read(zox_getv(mesh, ColorsGPULink), 0, vertices * 2 * sizeof(uint), lights) &&
            zox_gpu_storage_read(zox_getv(mesh, MeshGPULink).y, 0, vertices * sizeof(float4), positions))
        {
            for (uint i = 0; i < vertices; i += 4) {
                // each corner the brighter of the sky's and blocks' (as the cpu's colours, chunk_mesh_light)
                uint face = 0;
                for (byte k = 0; k < 4; k++) {
                    const uint sky = (lights[i * 2] >> (k * 8)) & 255, block = (lights[i * 2 + 1] >> (k * 8)) & 255;
                    face += sky > block ? sky : block;
                }
                *sum += face;
                *count += 4;
                const byte direction = (((int) (positions[i].w + 0.5f)) >> 8) & 7;
                zox_tst_cmf_face_light(world, mesh, chunk, positions[i], direction, (byte) (face / 4));
                if (direction == 3) {
                    *up_sum += face;
                    *up_count += 4;
                }
            }
        }
        free(lights);
        free(positions);
        return;
    }
    const MeshColors* colors = zox_get(mesh, MeshColors);
    for (int i = 0; colors->value && i < colors->length; i++) {
        const color c = colors->value[i];
        *sum += c.r > c.g ? c.r : c.g;     // its light: the brighter of the sky's (r) and blocks' (g)
        (*count)++;
    }
}

byte zox_tst_cmf_now = 0;
byte zox_tst_cmf_stage = 0;
entity zox_tst_cmf_streamer = 0;
int zox_tst_cmf_leg = 0;
float zox_tst_cmf_leg_done = 0;     // chunks along this leg
int zox_tst_cmf_frames = 0, zox_tst_cmf_moving_frames = 0;
int zox_tst_cmf_blanks = 0, zox_tst_cmf_partials = 0, zox_tst_cmf_swaps = 0, zox_tst_cmf_changed = 0;
int zox_tst_cmf_seam_frames = 0, zox_tst_cmf_logged = 0;
int zox_tst_cmf_by_stage[3][3];     // blanks, partials, frames with seams; loading (1) and walking (2)
zox_tst_seam_result zox_tst_cmf_seams = { 0 };
byte zox_tst_cmf_last_read = 0;
uint zox_tst_cmf_last_indices = 0, zox_tst_cmf_last_wanted = 0, zox_tst_cmf_last_room = 0;   // the last gpu mesh read

static inline zox_tst_cmf_slot* zox_tst_cmf_find(zox_tst_cmf_slot* map, entity key) {
    uint index = (uint) ((key * 0x9e3779b97f4a7c15ULL) >> 49) & (zox_tst_cmf_slots - 1);
    while (map[index].key && map[index].key != key) {
        index = (index + 1) & (zox_tst_cmf_slots - 1);
    }
    return &map[index];
}

// the faces a mesh draws this frame: gpu its command's index count read back (0 when the renderer skips it), cpu its
//  index count; partial set when a gpu mesh's faces didn't fit its room
static inline uint zox_tst_cmf_mesh_faces(ecs* world, entity mesh, byte* partial) {
    if (!zox_getv(mesh, MeshRenderCount)) {
        return 0;
    }
    if (!zox_has(mesh, GpuMesh)) {
        return zox_getv(mesh, MeshRenderCount) / 6;
    }
    if (!zox_getv(mesh, VaoGPULink)) {
        return 0;
    }
    const guint2 command = zox_getv(mesh, MeshCommandGPULink);
    uint indices = 0, wanted = 0;
    zox_tst_cmf_last_read = zox_gpu_storage_read(command.x, 0, sizeof(uint), &indices);
    const uint room = zox_getv(mesh, MeshFaceRoom);
    if (zox_getv(mesh, MeshFence)) {
        // meshed again, its count not read yet: it drew what fit
        zox_gpu_storage_read(command.y, 0, sizeof(uint), &wanted);
        *partial |= wanted > room;
    }
    zox_tst_cmf_last_indices = indices;
    zox_tst_cmf_last_wanted = wanted;
    zox_tst_cmf_last_room = room;
    const uint faces = indices / 6;
    return faces < room ? faces : room;
}

// a chunk whose lights are all dark with sunlight right above air in it (sunlit air on the bottom of the chunk above,
//  over air at its top): its lights cleared and never lit again
static inline byte zox_tst_cmf_unlit_under_sun(ecs* world, entity chunk) {
    const LightOctree* lights = zox_get(chunk, LightOctree);
    if (has_children_LightOctree(lights) || lights->value > darklight) {
        return 0;
    }
    const entity above = zox_get(chunk, ChunkNeighbors)->value[direction_up];
    if (!zox_valid(above) || !zox_has(above, LightOctree)) {
        return 0;
    }
    const LightOctree* above_lights = zox_get(above, LightOctree);
    const VoxelNode* above_voxels = zox_get(above, VoxelNode);
    const byte depth = zox_getv(above, NodeDepth);
    const int size = octree_size(depth);
    // and air under it here, its top voxel (a sunbeam lands on it): sunlight over solid ground lights nothing below
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    const byte my_depth = zox_getv(chunk, NodeDepth);
    const int my_size = octree_size(my_depth);
    for (int x = 0; x < size; x++) {
        for (int z = 0; z < size; z++) {
            const byte3 at = { x, 0, z };
            if (getv_VoxelNode(above_voxels, depth, at) || getv_LightOctree(above_lights, depth, at) < sunlight) {
                continue;
            }
            const int mx = my_depth >= depth ? x << (my_depth - depth) : x >> (depth - my_depth);
            const int mz = my_depth >= depth ? z << (my_depth - depth) : z >> (depth - my_depth);
            if (!getv_VoxelNode(voxels, my_depth, (byte3) { mx, my_size - 1, mz })) {
                return 1;
            }
        }
    }
    return 0;
}

// its light tree in its middle column, the cell over its ground at each depth (the light its meshes read there)
static inline void zox_tst_cmf_light_probe(ecs* world, entity chunk, char* text, int length) {
    const byte depth = zox_getv(chunk, NodeDepth);
    const int size = octree_size(depth);
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    const LightOctree* lights = zox_get(chunk, LightOctree);
    const int x = size / 2, z = size / 2;
    int ground = -1;
    for (int y = size - 1; y >= 0 && ground < 0; y--) {
        if (getv_VoxelNode(voxels, depth, (byte3) { x, y, z })) {
            ground = y;
        }
    }
    int n = snprintf(text, length, "light tree root [%i] open [%i], ground at [%i], over it by depth:", lights->value,
        has_children_LightOctree(lights), ground);
    const entity above = zox_get(chunk, ChunkNeighbors)->value[direction_up];
    if (zox_valid(above) && zox_has(above, LightOctree)) {
        const LightOctree* above_lights = zox_get(above, LightOctree);
        const byte above_depth = zox_getv(above, NodeDepth);
        const int above_size = octree_size(above_depth);
        int solid = 0, sunlit = 0, dark_air = 0;
        const VoxelNode* above_voxels = zox_get(above, VoxelNode);
        for (int ax = 0; ax < above_size; ax++) {
            for (int az = 0; az < above_size; az++) {
                const byte3 at = { ax, 0, az };
                if (getv_VoxelNode(above_voxels, above_depth, at)) {
                    solid++;
                } else if (getv_LightOctree(above_lights, above_depth, at) >= sunlight) {
                    sunlit++;
                } else {
                    dark_air += getv_LightOctree(above_lights, above_depth, at) <= darklight;
                }
            }
        }
        n += snprintf(text + n, length - n, " above's bottom row: solid [%i] sunlit [%i] dark air [%i] of [%i];",
            solid, sunlit, dark_air, above_size * above_size);
        n += snprintf(text + n, length - n, " above: depth [%i] root [%i] open [%i] its bottom middle [%i] generate [%i] lod dirty [%i];",
            above_depth, above_lights->value, has_children_LightOctree(above_lights),
            getv_LightOctree(above_lights, above_depth, (byte3) { above_size / 2, 0, above_size / 2 }),
            zox_has(above, GenerateChunk) ? zox_getv(above, GenerateChunk) : -1,
            zox_has(above, ChunkLodDirty) ? zox_getv(above, ChunkLodDirty) : -1);
    }
    if (ground < 0 || ground + 1 >= size) {
        return;
    }
    for (int d = depth; d >= 0 && n < length - 16; d--) {
        const int shift = depth - d;
        n += snprintf(text + n, length - n, " d%i [%i]", d, getv_LightOctree(lights, d,
            (byte3) { x >> shift, (ground + 1) >> shift, z >> shift }));
    }
}

// its meshes as the renderer sees them, d depth, D disabled, X build disabled, B building, M uploading, + built,
//  F a gpu count pending, c its count
static inline void zox_tst_cmf_log(ecs* world, const char* what, entity chunk, entity material, uint before) {
    if (zox_tst_cmf_stage != 2 || zox_tst_cmf_logged++ >= zox_tst_cmf_logs) {
        return;     // the walk's (loading's are counted)
    }
    char text[512];
    const int3 p = zox_getv(chunk, ChunkPosition);
    int n = snprintf(text, sizeof(text), "frame [%i] stage [%i] %s: chunk [%ix%ix%i] material [%i] lod dirty [%i] depth [%i] drew [%u] faces "
        "(last gpu read [%i]: indices [%u] wanted [%u] room [%u]) generate [%i] node depth [%i] voxels [%s%i], meshes:",
        zox_tst_cmf_frames, zox_tst_cmf_stage, what, p.x, p.y, p.z, zox_getv(material, BlockMaterial), zox_has(chunk, ChunkLodDirty) ? zox_getv(chunk, ChunkLodDirty) : -1,
        zox_getv(chunk, RenderDepth), before, zox_tst_cmf_last_read, zox_tst_cmf_last_indices, zox_tst_cmf_last_wanted, zox_tst_cmf_last_room,
        zox_has(chunk, GenerateChunk) ? zox_getv(chunk, GenerateChunk) : -1, zox_getv(chunk, NodeDepth),
        has_children_VoxelNode(zox_get(chunk, VoxelNode)) ? "split " : "closed ", zox_get(chunk, VoxelNode)->value);
    entity meshes[zox_chunk_meshes_max];
    const uint count = zox_get_children_by_id(world, material, meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
    for (uint j = 0; j < count && n < 480; j++) {
        const entity m = meshes[j];
        n += snprintf(text + n, sizeof(text) - n, " [d%i %s%s%s%s%s%s c%u]", zox_getv(m, RenderDepth),
            zox_has(m, Disabled) ? "D" : "", zox_has(m, BuildDisabled) ? "X" : "", zox_has(m, BuildMesh) ? "B" : "",
            zox_has(m, MeshDirty) ? "M" : "", zox_has(m, MeshBuilt) ? "+" : "",
            zox_has(m, GpuMesh) && zox_getv(m, MeshFence) ? "F" : "", zox_getv(m, MeshRenderCount));
    }
    zox_log("[tst]  %s", text);
}

double zox_tst_cmf_spent_ms = 0;  // the test's own time, its checks (reads back gpu meshes)

// a material's shown meshes' faces not on its chunk's borders
static inline int zox_tst_cmf_inside_faces(ecs* world, entity terrain, entity material) {
    float block_scale = 1;
    const byte resolution = zox_tst_seam_resolution(world, terrain, &block_scale);
    const int side = 1 << resolution;
    byte* cover = calloc(6 * side * side, 1);
    entity meshes[zox_chunk_meshes_max];
    const uint count = zox_get_children_by_id(world, material, meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
    int inside_all = 0;
    for (uint k = 0; k < count; k++) {
        if (zox_has(meshes[k], Disabled)) {
            continue;
        }
        int inside = 0;
        if (zox_tst_seam_mesh_faces(world, meshes[k], resolution, block_scale, cover, &inside) > 0) {
            inside_all += inside;
        }
    }
    free(cover);
    return inside_all;
}

// each chunk's state as it changes (its lod swap, sides, the meshes it shows), the last zox_tst_cmf_history_max
//  changes kept: a seam logs its two chunks' (zox_tst_cmf_seam_more), what each did in the frames before
#define zox_tst_cmf_history_max 8192
typedef struct {
    int frame;
    entity chunk;
    char text[96];
} zox_tst_cmf_change;
zox_tst_cmf_change zox_tst_cmf_history[zox_tst_cmf_history_max];
int zox_tst_cmf_history_count = 0;
zox_tst_cmf_slot zox_tst_cmf_states[zox_tst_cmf_slots];    // a chunk's last state's hash

// a chunk's state: lod dirty, render depth, generating, sides building; each material's shown meshes: depth and
//  flags (B building, M uploading, X held, + built)
static inline void zox_tst_cmf_state(ecs* world, entity chunk, char* text, int length) {
    int n = snprintf(text, length, "lod %i rd %i gen %i |",
        zox_has(chunk, ChunkLodDirty) ? zox_getv(chunk, ChunkLodDirty) : -1, zox_getv(chunk, RenderDepth),
        zox_has(chunk, GenerateChunk) ? zox_getv(chunk, GenerateChunk) : 0);
    entity materials[zox_block_materials];
    const uint materials_count = get_chunk_materials(world, chunk, materials);
    for (uint j = 0; j < materials_count && n < length - 16; j++) {
        n += snprintf(text + n, length - n, " m%u:", j);
        entity meshes[zox_chunk_meshes_max];
        const uint count = zox_get_children_by_id(world, materials[j], meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
        for (uint k = 0; k < count && n < length - 10; k++) {
            const entity m = meshes[k];
            if (zox_has(m, Disabled)) {
                continue;
            }
            n += snprintf(text + n, length - n, "d%i%s%s%s%s ", zox_getv(m, RenderDepth), zox_has(m, BuildMesh) ? "B" : "",
                zox_has(m, MeshDirty) ? "M" : "", zox_has(m, BuildDisabled) ? "X" : "", zox_has(m, MeshBuilt) ? "+" : "");
        }
    }
}

static inline void zox_tst_cmf_record(ecs* world, entity chunk) {
    char text[96];
    zox_tst_cmf_state(world, chunk, text, sizeof(text));
    uint hash = 2166136261u;
    for (const char* c = text; *c; c++) {
        hash = (hash ^ (byte) *c) * 16777619u;
    }
    zox_tst_cmf_slot* last = zox_tst_cmf_find(zox_tst_cmf_states, chunk);
    if (last->value == hash) {
        return;
    }
    last->value = hash;
    zox_tst_cmf_change* change = &zox_tst_cmf_history[zox_tst_cmf_history_count++ % zox_tst_cmf_history_max];
    change->frame = zox_tst_cmf_frames;
    change->chunk = chunk;
    snprintf(change->text, sizeof(change->text), "%s", text);
}

// a chunk's last changes, oldest first
static inline void zox_tst_cmf_history_log(entity chunk, const char* who) {
    int found[12], count = 0;
    for (int i = zox_tst_cmf_history_count - 1; i >= 0 && i >= zox_tst_cmf_history_count - zox_tst_cmf_history_max && count < 12; i--) {
        if (zox_tst_cmf_history[i % zox_tst_cmf_history_max].chunk == chunk) {
            found[count++] = i % zox_tst_cmf_history_max;
        }
    }
    for (int k = count - 1; k >= 0; k--) {
        const zox_tst_cmf_change* c = &zox_tst_cmf_history[found[k]];
        zox_log("[tst]      %s frame [%i] %s", who, c->frame, c->text);
    }
}

static inline void zox_tst_cmf_frame(ecs* world, entity terrain) {
    const double started = get_time_ms();
    const zox_tst_cmf_slot* before = zox_tst_cmf_maps[zox_tst_cmf_now];
    zox_tst_cmf_slot* now = zox_tst_cmf_maps[!zox_tst_cmf_now];
    memset(now, 0, sizeof(zox_tst_cmf_maps[0]));
    zox_tst_seam_reset();
    int candidates = 0;
    iter it = zox_children(world, terrain);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity chunk = it.entities[i];
            if (!zox_has(chunk, Chunk3)) {
                continue;
            }
            zox_tst_cmf_record(world, chunk);
            uint signature = 0;
            entity materials[zox_block_materials];
            const uint materials_count = get_chunk_materials(world, chunk, materials);
            for (uint j = 0; j < materials_count; j++) {
                const entity material = materials[j];
                entity meshes[zox_chunk_meshes_max];
                const uint count = zox_get_children_by_id(world, material, meshes, zox_chunk_meshes_max, zox_id(ChunkMesh));
                uint faces = 0;
                byte partial = 0, settled_empty = 0;
                double light_sum = 0, up_sum = 0;
                uint light_count = 0, up_count = 0;
                byte light_read = 0;
                entity first_shown = 0;
                for (uint k = 0; k < count; k++) {
                    const entity mesh = meshes[k];
                    if (zox_has(mesh, Disabled)) {
                        continue;
                    }
                    const uint mesh_faces = zox_tst_cmf_mesh_faces(world, mesh, &partial);
                    faces += mesh_faces;
                    if (!first_shown) {
                        first_shown = mesh;
                    }
                    // its light read back only while it can change (building, relighting, uploading, its count back, its
                    //  chunk swapping), else as it was (reading every mesh every frame took minutes)
                    const byte changing = zox_has(mesh, BuildMesh) || zox_has(mesh, MeshDirty) || zox_has(mesh, BuildMeshColors) ||
                        zox_has(mesh, MeshColorsDirty) || (zox_has(mesh, GpuMesh) && zox_getv(mesh, MeshFence)) ||
                        zox_has(chunk, ChunkLodDirty);
                    if (!zox_no_lights && zox_has(mesh, MeshColorsBuilt) && (changing || zox_tst_cmf_light_find(material)->mesh != mesh)) {
                        zox_tst_cmf_mesh_light(world, mesh, chunk, mesh_faces, &light_sum, &light_count, &up_sum, &up_count);
                        light_read = 1;
                    }
                    signature = signature * 31 + (uint) mesh * 7 + mesh_faces;
                    settled_empty |= zox_has(mesh, MeshBuilt) && !zox_has(mesh, BuildMesh) && !zox_has(mesh, MeshDirty) &&
                        !(zox_has(mesh, GpuMesh) && zox_getv(mesh, MeshFence));
                }
                // drew faces, now none: a blank once it draws faces again (none settled: its faces went, covered)
                const uint last = zox_tst_cmf_find((zox_tst_cmf_slot*) before, material)->value;
                zox_tst_cmf_slot* pending = zox_tst_cmf_find(zox_tst_cmf_pending, material);
                pending->key = material;    // value: the frame it drew none since, + 1 (0: drawing)
                if (zox_tst_cmf_frames && last && !faces && !pending->value) {
                    pending->value = zox_tst_cmf_frames + 1;
                } else if (pending->value && faces) {
                    // its faces inside it came back (its border's are the seam check's: owed ones missing are seams,
                    //  the rest were behind a neighbour, unseen)
                    if (zox_tst_cmf_inside_faces(world, terrain, material)) {
                        zox_tst_cmf_blanks++;
                        zox_tst_cmf_by_stage[zox_tst_cmf_stage][0]++;
                        zox_tst_cmf_log(world, "blank, drew again", chunk, material, zox_tst_cmf_frames + 1 - pending->value);
                    }
                    pending->value = 0;
                } else if (pending->value && settled_empty) {
                    pending->value = 0;     // settled with none: covered
                }
                // its light: a dip (a dark flash that comes back), or a step as it swapped lods
                zox_tst_cmf_light* lit = zox_tst_cmf_light_find(material);
                // upward faces' light where it has them (the same surface across a swap), else all its faces'
                const float light = !light_read ? zox_tst_cmf_light_find(material)->light :
                    up_count ? (float) (up_sum / up_count) : light_count ? (float) (light_sum / light_count) : -1;
                if (zox_tst_cmf_frames && light >= 0 && lit->light > 16) {
                    if (lit->mesh && first_shown != lit->mesh) {
                        zox_tst_cmf_swaps_lit++;
                        // a fifth of the brighter and an eighth of full light: steps in the dark (16 to 45) don't show
                        if (fabsf(light - lit->light) > fmaxf(light, lit->light) * 0.2f && fabsf(light - lit->light) > 32) {
                            zox_tst_cmf_steps++;
                            // drawn dark on one side: a lod lit and one never lit (its lights cleared and not lit
                            //  again), what showed as lights flashing; others are lods' shapes (a coarse cell, one light)
                            const float darker = fminf(light, lit->light), brighter = fmaxf(light, lit->light);
                            zox_tst_cmf_dark_steps += darker <= darklight * 2 && brighter >= darker * 3 &&
                                zox_tst_cmf_unlit_under_sun(world, chunk);
                            static int logged_steps = 0;
                            if (logged_steps++ < 14) {
                                const int3 p = zox_getv(chunk, ChunkPosition);
                                byte neighbours_lighting = 0;
                                const ChunkNeighbors* ns = zox_get(chunk, ChunkNeighbors);
                                for (byte n = 0; n < chunk_neighbors_length; n++) {
                                    neighbours_lighting += zox_valid(ns->value[n]) && zox_chunk_lighting_busy(world, ns->value[n]);
                                }
                                zox_log("[tst]  light step: chunk [%ix%ix%i] material [%i] swapped lods d%i to d%i (node depth [%i]), its %s light [%.0f] to [%.0f], lighting [%i], neighbours lighting [%i]",
                                    p.x, p.y, p.z, zox_getv(material, BlockMaterial),
                                    zox_valid(lit->mesh) ? zox_getv(lit->mesh, RenderDepth) : -1, zox_getv(first_shown, RenderDepth),
                                    zox_getv(chunk, NodeDepth), up_count ? "upward faces'" : "faces'", lit->light, light,
                                    zox_chunk_lighting_busy(world, chunk), neighbours_lighting);
                                char probe[256];
                                zox_tst_cmf_light_probe(world, chunk, probe, sizeof(probe));
                                zox_log("[tst]    %s", probe);
                            }
                        }
                    } else if (!lit->dip_from && light < lit->light * 0.75f && lit->light - light > 32 &&
                        !block_material_liquid(zox_getv(material, BlockMaterial))) {
                        // (a liquid's surface changes as it flows, its faces' light with it)
                        lit->dip_from = lit->light;
                        lit->dip_frame = zox_tst_cmf_frames;
                        static int logged_dip_starts = 0;
                        if (logged_dip_starts++ < 6) {
                            const int3 p = zox_getv(chunk, ChunkPosition);
                            char text[256];
                            int n = snprintf(text, sizeof(text), "dip starts: chunk [%ix%ix%i] material [%i] [%.0f] to [%.0f], lights settling [%i], meshes",
                                p.x, p.y, p.z, zox_getv(material, BlockMaterial), lit->light, light, zox_chunk_lights_settling(world, chunk));
                            for (uint k = 0; k < count && n < 220; k++) {
                                const entity m = meshes[k];
                                n += snprintf(text + n, sizeof(text) - n, " [d%i%s%s%s%s%s%s]", zox_getv(m, RenderDepth),
                                    zox_has(m, Disabled) ? "D" : "", zox_has(m, BuildMesh) ? "B" : "", zox_has(m, MeshDirty) ? "M" : "",
                                    zox_has(m, BuildMeshColors) ? "L" : "", zox_has(m, MeshColorsBuilt) ? "+" : "",
                                    zox_has(m, GpuMesh) && zox_getv(m, MeshFence) ? "F" : "");
                            }
                            zox_log("[tst]  %s", text);
                        }
                    }
                }
                if (lit->dip_from && light >= 0) {
                    if (light >= lit->dip_from * 0.9f) {
                        zox_tst_cmf_dips++;
                        static int logged_dips = 0;
                        if (logged_dips++ < 6) {
                            const int3 p = zox_getv(chunk, ChunkPosition);
                            zox_log("[tst]  light dip: chunk [%ix%ix%i] material [%i] light [%.0f] fell and came back over [%i] frames",
                                p.x, p.y, p.z, zox_getv(material, BlockMaterial), lit->dip_from, zox_tst_cmf_frames - lit->dip_frame);
                        }
                        lit->dip_from = 0;
                    } else if (zox_tst_cmf_frames - lit->dip_frame > 15) {
                        lit->dip_from = 0;  // it stayed: a real change
                    }
                }
                if (light >= 0) {
                    lit->light = light;
                    lit->mesh = first_shown;
                }
                if (partial) {
                    zox_tst_cmf_partials++;
                    zox_tst_cmf_by_stage[zox_tst_cmf_stage][1]++;
                    zox_tst_cmf_log(world, "partial", chunk, material, faces);
                }
                zox_tst_cmf_slot* slot = zox_tst_cmf_find(now, material);
                slot->key = material;
                slot->value = faces;
            }
            // its drawing changed this frame: its borders are checked
            const uint last_signature = zox_tst_cmf_find((zox_tst_cmf_slot*) before, chunk)->value;
            zox_tst_cmf_slot* slot = zox_tst_cmf_find(now, chunk);
            slot->key = chunk;
            slot->value = signature ? signature : 1;
            if (zox_tst_cmf_frames && slot->value != last_signature) {
                zox_tst_seam_slot(chunk)->candidate = 1;
                candidates++;
                zox_tst_cmf_slot* changed = zox_tst_cmf_find(zox_tst_cmf_changed_at, chunk);
                changed->key = chunk;
                changed->value = zox_tst_cmf_frames;
            }
            zox_tst_cmf_swaps += zox_has(chunk, ChunkLodDirty);
        }
    }
    zox_tst_cmf_changed += candidates;
    if (candidates && !zox_tst_cmf_lights_only) {
        zox_tst_seam_result frame = { 0 };
        const int logged = zox_tst_seam_logged;
        zox_tst_seam_check(world, terrain, 1, &frame);
        zox_tst_cmf_seam_frames += frame.missing > 0;
        zox_tst_cmf_by_stage[zox_tst_cmf_stage][2] += frame.missing > 0;
        if (frame.missing && logged < zox_tst_seam_logs) {
            zox_log("[tst]  frame [%i]: [%i] seam cells in [%i] borders, [%i] chunks changed", zox_tst_cmf_frames,
                frame.missing, frame.seam_pairs, candidates);
        }
        zox_tst_cmf_seams.pairs += frame.pairs;
        zox_tst_cmf_seams.lod_pairs += frame.lod_pairs;
        zox_tst_cmf_seams.missing += frame.missing;
        zox_tst_cmf_seams.missing_lod += frame.missing_lod;
        zox_tst_cmf_seams.seam_pairs += frame.seam_pairs;
        zox_tst_cmf_seams.unready += frame.unready;
        zox_tst_cmf_seams.unknown += frame.unknown;
    }
    zox_tst_seam_reset();
    zox_tst_cmf_now = !zox_tst_cmf_now;
    zox_tst_cmf_frames++;
    zox_tst_cmf_spent_ms += get_time_ms() - started;
    if (zox_tst_cmf_frames % 100 == 0) {
        zox_log("[tst] frame [%i] leg [%i] at [%.1f], its checks took [%.1fs], [%i] chunks changed this frame", zox_tst_cmf_frames,
            zox_tst_cmf_leg, zox_tst_cmf_leg_done, zox_tst_cmf_spent_ms / 1000.0, candidates);
    }
}

// a chunk's state, its meshes: d depth, D disabled, X build disabled, B building, M uploading, + built
static inline int zox_tst_cmf_chunk_text(ecs* world, entity chunk, char* text, int length) {
    int n = snprintf(text, length, "changed [%i frames ago] lod dirty [%i] generate [%i] lighting [%i: dirty %i sun %i light %i dark %i] node depth [%i] meshes",
        zox_tst_cmf_frames - (int) zox_tst_cmf_find(zox_tst_cmf_changed_at, chunk)->value,
        zox_has(chunk, ChunkLodDirty) ? zox_getv(chunk, ChunkLodDirty) : -1,
        zox_has(chunk, GenerateChunk) ? zox_getv(chunk, GenerateChunk) : -1, zox_chunk_lighting_busy(world, chunk),
        zox_has(chunk, LightOctreeDirty) ? zox_getv(chunk, LightOctreeDirty) : -1, (int) zox_get(chunk, SunlightQueue)->count,
        (int) zox_get(chunk, LightQueue)->count, (int) zox_get(chunk, DarkQueue)->count,
        zox_getv(chunk, NodeDepth));
    entity meshes[zox_chunk_meshes_max];
    const uint count = get_chunk_meshes(world, chunk, meshes);
    for (uint j = 0; j < count && n < length - 24; j++) {
        const entity m = meshes[j];
        n += snprintf(text + n, length - n, " [d%i%s%s%s%s%s]", zox_getv(m, RenderDepth), zox_has(m, Disabled) ? "D" : "",
            zox_has(m, BuildDisabled) ? "X" : "", zox_has(m, BuildMesh) ? "B" : "", zox_has(m, MeshDirty) ? "M" : "",
            zox_has(m, MeshBuilt) ? "+" : "");
    }
    return n;
}

static void zox_tst_cmf_seam_more(ecs* world, entity x, entity y, char* text, int length) {
    int n = snprintf(text, length, "it: ");
    n += zox_tst_cmf_chunk_text(world, x, text + n, length - n);
    n += snprintf(text + n, length - n, " | neighbour: ");
    zox_tst_cmf_chunk_text(world, y, text + n, length - n);
    // what each did before (logged straight away, before the caller's line)
    zox_log("[tst]    now frame [%i], their changes:", zox_tst_cmf_frames);
    zox_tst_cmf_history_log(x, "it       ");
    zox_tst_cmf_history_log(y, "neighbour");
}

byte zox_tst_chunk_mesh_flicker_ready(ecs* world) {
    zox_test_capture_mesh_border = !zox_tst_cmf_lights_only;
    zox_tst_seam_log_more = zox_tst_cmf_seam_more;
    const char* speed = getenv("zox_tst_flicker_speed");
    if (speed && speed[0]) {
        zox_tst_cmf_speed = atof(speed);
    }
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    if (zox_tst_cmf_stage == 0) {
        zox_tst_cmf_streamer = spawn_streamer(world, prefab_streamer, terrain, (float3) { chunk * 0.5f, chunk, chunk * 0.5f });
        zox_setv(zox_tst_cmf_streamer, StreamerLevel, 1);
        zox_tst_cmf_stage = 1;
        return 0;
    }
    zox_tst_cmf_frame(world, terrain);
    zox_tst_progress(zox_tst_cmf_leg * 1000 + (int) (zox_tst_cmf_leg_done * 100) + zox_tst_cmf_frames);
    // the land loaded first (checked as it loads), then it walks
    if (zox_tst_cmf_stage == 1) {
        zox_tst_waiting = "the land to load";
        if (zox_tst_cmf_frames > 10 && !zox_tst_terrain_busy(world, terrain)) {
            zox_tst_cmf_stage = 2;
        }
        return 0;
    }
    if (zox_tst_cmf_leg >= zox_tst_cmf_legs) {
        return 1;
    }
    zox_tst_waiting = "the streamer to walk its path";
    const float3 leg = zox_tst_cmf_path[zox_tst_cmf_leg];
    const float length = fabsf(leg.x) + fabsf(leg.y) + fabsf(leg.z);
    float step = zox_tst_cmf_speed * zox_delta_time;
    if (zox_tst_cmf_leg_done + step > length) {
        step = length - zox_tst_cmf_leg_done;
    }
    const float3 direction = float3_scale(leg, 1.0f / length);
    float3 position = zox_getv(zox_tst_cmf_streamer, Position3D);
    position = float3_add(position, float3_scale(direction, step * chunk));
    zox_setv(zox_tst_cmf_streamer, Position3D, position);
    zox_tst_cmf_leg_done += step;
    zox_tst_cmf_moving_frames++;
    if (zox_tst_cmf_leg_done >= length - 0.0001f) {
        zox_tst_cmf_leg++;
        zox_tst_cmf_leg_done = 0;
    }
    return 0;
}

void zox_tst_chunk_mesh_flicker_check(ecs* world) {
    (void) world;
    const zox_tst_seam_result* s = &zox_tst_cmf_seams;
    zox_log("[tst] [metric] chunk_mesh_flicker [%s meshing] frames [%i] moving [%i]: blanks [%i] partial gpu meshes [%i], "
        "seams [%i] cells in [%i] borders over [%i] frames ([%i] between lods), borders checked [%i] ([%i] between lods), "
        "chunk changes [%i], frames chunks swapped lods [%i], its checks took [%.1fs]",
        zox_chunks3_gpu_meshing ? "gpu" : "cpu", zox_tst_cmf_frames, zox_tst_cmf_moving_frames, zox_tst_cmf_blanks,
        zox_tst_cmf_partials, s->missing, s->seam_pairs, zox_tst_cmf_seam_frames, s->missing_lod, s->pairs, s->lod_pairs,
        zox_tst_cmf_changed, zox_tst_cmf_swaps, zox_tst_cmf_spent_ms / 1000.0);
    zox_log("[tst] [metric] chunk_mesh_flicker loading: blanks [%i] partials [%i] seam frames [%i], walking: blanks [%i] partials [%i] seam frames [%i]",
        zox_tst_cmf_by_stage[1][0], zox_tst_cmf_by_stage[1][1], zox_tst_cmf_by_stage[1][2],
        zox_tst_cmf_by_stage[2][0], zox_tst_cmf_by_stage[2][1], zox_tst_cmf_by_stage[2][2]);
    zox_tst_check(zox_tst_cmf_leg >= zox_tst_cmf_legs, "the walk stopped at leg [%i]", zox_tst_cmf_leg);
    // lods changed (at far 16 the land near the walk is one lod: no borders between two, its swaps still happen)
    zox_tst_check(zox_tst_cmf_swaps > 0, "no lod changes seen, nothing tested");
    zox_tst_check(!zox_tst_cmf_blanks, "[%i] times a chunk's material drew nothing the frame after it drew (a blank)", zox_tst_cmf_blanks);
    zox_tst_check(!zox_tst_cmf_partials, "[%i] times a gpu mesh drew only the faces that fit its room", zox_tst_cmf_partials);
    zox_log("[tst] [metric] chunk_mesh_flicker lights: dips [%i], steps [%i] of [%i] lod swaps, [%i] to or from a lod drawn dark",
        zox_tst_cmf_dips, zox_tst_cmf_steps, zox_tst_cmf_swaps_lit, zox_tst_cmf_dark_steps);
    zox_log("[tst] [metric] chunk_mesh_flicker faces: [%i] seen walking, [%i] flashed dark and back, [%i] showed dark then lit",
        zox_tst_cmf_faces_seen, zox_tst_cmf_face_flashes, zox_tst_cmf_face_late);
    for (int i = 0; i < zox_tst_cmf_flash_chunks_count; i++) {
        const entity c = zox_tst_cmf_flash_chunks[i];
        if (zox_valid(c)) {
            const int3 p = zox_getv(c, ChunkPosition);
            zox_log("[tst]  a face flashed in chunk [%ix%ix%i]", p.x, p.y, p.z);
        }
    }
    zox_tst_check(!zox_tst_cmf_face_flashes, "[%i] faces flashed dark and came back", zox_tst_cmf_face_flashes);
    zox_tst_check(!zox_tst_cmf_face_late, "[%i] faces showed dark then lit", zox_tst_cmf_face_late);
    // a material's mean light (dips) is logged, not failed: it moves as its meshes' faces change as much as with a
    //  flash (849 dips on the cpu meshes, no flash seen in play, 2026-11-24); each face's own light (the checks above,
    //  gpu meshing) is what a flash is
    zox_tst_check(!zox_tst_cmf_dark_steps, "[%i] lod swaps to or from a lod drawn dark, its lights never lit under sunlight",
        zox_tst_cmf_dark_steps);
    zox_tst_check(!s->missing, "[%i] missing seam cells in [%i] frames, [%i] at LOD boundaries",
        s->missing, zox_tst_cmf_seam_frames, s->missing_lod);
}

// tst: lod_light_flash, chunk_mesh_flicker's walk for its lights alone, as a player flies (far 16, tst.sh, at 3 chunks
//  a second, gpu meshing): far chunks generated coarse grow finer as it comes (their voxels again, their lights cleared)
//  - checks the drawn faces for rapid darkening and recovery or rapid brightening. Unloading a neighbour
//    used to replace its shade with full sunlight on upward faces at the stream boundary.
//  - also checks for grids baked while their light sources settle and for a never-lit LOD.
//  - gpu meshing on (its setup), any backend: the count is the mesher's own (the flash was seen on vulkan)
//  - at medium it never happens (every chunk's generated fine from the start): it's far 16 or nothing
void zox_tst_lod_light_flash_setup() {
    zox_tst_cmf_lights_only = 1;
    zox_tst_cmf_speed = 3;
    zox_chunks3_gpu_meshing = 1;
}

void zox_tst_lod_light_flash_check(ecs* world) {
    (void) world;
    zox_log("[tst] [metric] lod_light_flash frames [%i] moving [%i], lod swaps seen [%i], faces [%i]: [%i] shown dark then lit, [%i] flashed dark and back, dips [%i], lods never lit [%i]",
        zox_tst_cmf_frames, zox_tst_cmf_moving_frames, zox_tst_cmf_swaps, zox_tst_cmf_faces_seen, zox_tst_cmf_face_late,
        zox_tst_cmf_face_flashes, zox_tst_cmf_dips, zox_tst_cmf_dark_steps);
    const float3 at = zox_valid(zox_tst_cmf_streamer) ? zox_getv(zox_tst_cmf_streamer, Position3D) : float3_zero;
    for (int i = 0; i < zox_tst_cmf_flash_chunks_count; i++) {
        const entity c = zox_tst_cmf_flash_chunks[i];
        if (zox_valid(c)) {
            const int3 p = zox_getv(c, ChunkPosition);
            zox_log("[tst]  a face drawn dark then lit in chunk [%ix%ix%i] depth [%i] node depth [%i], the walk's end at [%.0f %.0f]",
                p.x, p.y, p.z, zox_getv(c, RenderDepth), zox_getv(c, NodeDepth), at.x, at.z);
        }
    }
    zox_tst_check(zox_tst_cmf_leg >= zox_tst_cmf_legs, "the walk stopped at leg [%i]", zox_tst_cmf_leg);
    zox_tst_check(zox_tst_cmf_swaps > 0, "no lod changes seen, nothing tested");
    zox_tst_check(zox_tst_cmf_faces_seen > 10000, "only [%i] faces read, too few to tell", zox_tst_cmf_faces_seen);
    // its cause, not its look: a shown mesh's lights baked while the lights round it settled (deterministic: the faces'
    //  light as drawn, logged above, moved with timing, light arriving later counted with the flashes)
    zox_log("[tst] [metric] lod_light_flash shown meshes baked as the lights round them settled [%u]", zox_chunk_gpu_meshes_baked_dark);
    zox_tst_check(!zox_tst_cmf_face_flashes, "[%i] faces flashed dark and back", zox_tst_cmf_face_flashes);
    zox_tst_check(!zox_tst_cmf_face_late, "[%i] faces showed dark then lit", zox_tst_cmf_face_late);
    zox_tst_check(!zox_chunk_gpu_meshes_baked_dark, "[%u] shown meshes baked their lights while the lights round them settled (drawn dark, then lit)",
        zox_chunk_gpu_meshes_baked_dark);
    zox_tst_check(!zox_tst_cmf_dark_steps, "[%i] lod swaps to or from a lod never lit under sunlight", zox_tst_cmf_dark_steps);
}

void zox_tsts_chunk_mesh_flicker(ecs* world) {
    zox_tst_add("lod_light_flash", zox_tst_chunk_mesh_flicker_ready, zox_tst_lod_light_flash_check, 600);
    zox_tst_set_setup("lod_light_flash", zox_tst_lod_light_flash_setup, 1);
    (void) world;
    zox_tst_add("chunk_mesh_flicker", zox_tst_chunk_mesh_flicker_ready, zox_tst_chunk_mesh_flicker_check, 240);
    zox_tst_set_setup("chunk_mesh_flicker", NULL, 1);
}
