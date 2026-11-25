// tst: chunk_uvs, a textured chunk's meshes draw their blocks' textures, one mesh a material (chunks3/fun/materials.c)
//  - a chunk3 parent's top middle chunk (chunk3_parent.c), cpu meshing: every regular block of the realm (a solid cube,
//    drawn as a cube: not a vox, a filler or bigger than a block) apart in a grid, and two brick cups, water in one, oil in the other (the walls
//    keep them, nothing flows)
//  - once its meshes are built and shown, each material's mesh:
//      - solid: drawn from the tilemap (MaterialLink); each face, its voxel and direction found from its vertices, has
//          its block's uvs for that direction (TilemapUVs, a block's 6 sides 4 uvs each), exactly: grass's top isn't
//          its side; and every block placed drew a face
//      - water, oil: a see through mesh of its own (TransparentMesh), its own texture (MeshTextureLink, the block's
//          Texture), its uvs within it
//  - run: bash bsh/tst.sh chunk_uvs
byte zox_tst_cuvs_stage = 0;
entity zox_tst_cuvs_chunk = 0;
byte zox_tst_cuvs_blocks[3];    // bricks, water, oil
byte zox_tst_cuvs_regular[64];  // every regular block, placed at zox_tst_cuvs_place(i)
byte zox_tst_cuvs_regular_count = 0;

// where regular block i goes: a grid a block apart, local y 8 and 11, clear of the cups
static inline byte3 zox_tst_cuvs_place(byte i) {
    return (byte3) { 1 + 2 * (i % 7), 8 + 3 * (i / 49), 1 + 2 * ((i / 7) % 7) };
}
const byte zox_tst_cuvs_materials[3] = { zox_block_material_solid, zox_block_material_liquid, zox_block_material_oil };

// a cup of bricks, a block of liquid in it at local (x, 4, z)
static inline void zox_tst_cuvs_cup(VoxelNodeQueue* queue, byte x, byte z, byte bricks, byte liquid) {
    const byte3 sides[5] = { { x, 3, z }, { x - 1, 4, z }, { x + 1, 4, z }, { x, 4, z - 1 }, { x, 4, z + 1 } };
    for (byte i = 0; i < 5; i++) {
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = bricks, .position = sides[i] });
    }
    a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = liquid, .position = (byte3) { x, 4, z } });
}

// the chunk's shown mesh of a material, built (its uvs in), 0 if none yet
static inline entity zox_tst_cuvs_mesh(ecs* world, byte material) {
    entity meshes[zox_chunk_meshes_max];
    const uint count = get_chunk_meshes(world, zox_tst_cuvs_chunk, meshes);
    for (uint i = 0; i < count; i++) {
        const entity mesh = meshes[i];
        const entity parent = zox_get_parent(world, mesh);
        if (!zox_valid(parent) || !zox_has(parent, BlockMaterial) || zox_getv(parent, BlockMaterial) != material ||
            zox_has(mesh, Disabled) || zox_has(mesh, BuildMesh) || zox_has(mesh, MeshDirty) || !zox_has(mesh, MeshUVs) ||
            !zox_get(mesh, MeshUVs)->length)
        {
            continue;
        }
        return mesh;
    }
    return 0;
}

byte zox_tst_chunk_uvs_ready(ecs* world) {
    if (!zox_tst_chunk3_parent_ready(world)) {
        return 0;
    }
    if (!zox_tst_cuvs_stage) {
        for (byte i = 0; i < 3; i++) {
            zox_tst_cuvs_blocks[i] = zox_tst_chunks_block(world, zox_tst_cuvs_materials[i], 0);
        }
        zox_tst_check(zox_tst_cuvs_blocks[0] && zox_tst_cuvs_blocks[1] && zox_tst_cuvs_blocks[2],
            "the realm has no bricks [%i], water [%i] or oil [%i]", zox_tst_cuvs_blocks[0], zox_tst_cuvs_blocks[1],
            zox_tst_cuvs_blocks[2]);
        if (!zox_tst_cuvs_blocks[0] || !zox_tst_cuvs_blocks[1] || !zox_tst_cuvs_blocks[2]) {
            return 1;
        }
        zox_tst_cuvs_chunk = zox_tst_chunk_at(world, (int3) { 0, 1, 0 });
        VoxelNodeQueue* queue = zox_getm(zox_tst_cuvs_chunk, VoxelNodeQueue);
        const BlockLinks* blocks = zox_get(zox_tst_chunks_realm, BlockLinks);
        for (int i = 0; i < blocks->length && zox_tst_cuvs_regular_count < 64; i++) {
            const entity block = blocks->value[i];
            if (!zox_valid(block) || zox_has(block, BlockVox) || zox_has(block, BlockSize) ||
                (zox_has(block, BlockModel) && zox_getv(block, BlockModel) != zox_block_solid) ||
                (zox_has(block, BlockMaterial) && zox_getv(block, BlockMaterial) != zox_block_material_solid))
            {
                continue;
            }
            const byte index = zox_getv(block, BlockIndex);
            a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = index, .position = zox_tst_cuvs_place(zox_tst_cuvs_regular_count) });
            zox_tst_cuvs_regular[zox_tst_cuvs_regular_count++] = index;
        }
        zox_tst_cuvs_cup(queue, 4, 4, zox_tst_cuvs_blocks[0], zox_tst_cuvs_blocks[1]);
        zox_tst_cuvs_cup(queue, 10, 10, zox_tst_cuvs_blocks[0], zox_tst_cuvs_blocks[2]);
        zox_tst_cuvs_stage = 1;
        return 0;
    }
    zox_tst_waiting = "a shown mesh for the bricks, the water and the oil";
    for (byte i = 0; i < 3; i++) {
        if (!zox_tst_cuvs_mesh(world, zox_tst_cuvs_materials[i])) {
            return 0;
        }
    }
    return !zox_tst_chunks_busy(world, zox_tst_chunks_parent);
}

// the realm block of a BlockIndex
static inline entity zox_tst_cuvs_block(ecs* world, byte index) {
    return zox_get(zox_tst_chunks_realm, BlockLinks)->value[index - 1];
}

void zox_tst_chunk_uvs_check(ecs* world) {
    zox_tst_check(zox_tst_cuvs_stage == 1, "the chunk3 parent never got lit");
    if (zox_tst_cuvs_stage != 1) {
        return;
    }
    // solid: from the tilemap, each face its block's uvs for its direction
    const entity solid = zox_tst_cuvs_mesh(world, zox_block_material_solid);
    const entity tilemap = zox_get_link(world, solid, MaterialLink);
    zox_tst_check(zox_valid(tilemap) && zox_has(tilemap, TilemapUVs), "the solid mesh isn't drawn from a tilemap");
    if (zox_valid(tilemap) && zox_has(tilemap, TilemapUVs)) {
        const TilemapUVs* tiles = zox_get(tilemap, TilemapUVs);
        const MeshUVs* uvs = zox_get(solid, MeshUVs);
        const MeshVertices* vertices = zox_get(solid, MeshVertices);
        const VoxelNode* voxels = zox_get(zox_tst_cuvs_chunk, VoxelNode);
        const byte depth = zox_getv(zox_tst_cuvs_chunk, NodeDepth);
        const int length = octree_size(depth);
        const float scale = zox_getv(zox_tst_cuvs_chunk, BlockScale);
        byte drew[64] = { 0 };
        int faces = 0, unplaced = 0, wrong = 0;
        for (int f = 0; f + 4 <= uvs->length && f + 4 <= vertices->length; f += 4, faces++) {
            // its direction and voxel: the face's vertices are its voxel's corner plus that direction's, scaled; a face
            //  fits its voxel and the one it faces (the other way round), the solid one is its
            int direction = -1;
            int3 voxel = int3_zero;
            byte block = 0;
            for (byte d = 0; d < 6 && !block; d++) {
                const float3 corner = float3_scale(float3_subtract(vertices->value[f], float3_scale(voxel_face_vertices_n[d][0], scale)), 1 / scale);
                const int3 v = (int3) { (int) roundf(corner.x), (int) roundf(corner.y), (int) roundf(corner.z) };
                byte fits = v.x >= 0 && v.y >= 0 && v.z >= 0 && v.x < length && v.y < length && v.z < length;
                for (byte k = 0; k < 4 && fits; k++) {
                    const float3 expect = float3_scale(float3_add(float3_from_int3(v), voxel_face_vertices_n[d][k]), scale);
                    fits = float3_distance(expect, vertices->value[f + k]) < 0.001f;
                }
                const byte at = fits ? getv_VoxelNode(voxels, depth, (byte3) { v.x, v.y, v.z }) : 0;
                const entity at_block = at ? zox_tst_cuvs_block(world, at) : 0;
                if (at && (!zox_has(at_block, BlockMaterial) || zox_getv(at_block, BlockMaterial) == zox_block_material_solid)) {
                    direction = d;
                    voxel = v;
                    block = at;
                }
            }
            if (!block) {
                unplaced++;
                continue;
            }
            const float2* expected = &tiles->value[(block - 1) * 24 + direction * 4];
            byte same = 1;
            for (byte k = 0; k < 4; k++) {
                same &= fabsf(uvs->value[f + k].x - expected[k].x) < 0.0001f && fabsf(uvs->value[f + k].y - expected[k].y) < 0.0001f;
            }
            if (!same) {
                if (wrong < 4) {
                    zox_log("[tst] chunk_uvs: block [%s] at [%ix%ix%i] direction [%i]: uv [%.3f %.3f], expected [%.3f %.3f]",
                        zox_getn(zox_tst_cuvs_block(world, block)), voxel.x, voxel.y, voxel.z, direction,
                        uvs->value[f].x, uvs->value[f].y, expected[0].x, expected[0].y);
                }
                wrong++;
            }
            for (byte i = 0; i < zox_tst_cuvs_regular_count; i++) {
                const byte3 at = zox_tst_cuvs_place(i);
                drew[i] |= at.x == voxel.x && at.y == voxel.y && at.z == voxel.z;
            }
        }
        int missing = 0;
        for (byte i = 0; i < zox_tst_cuvs_regular_count; i++) {
            if (!drew[i]) {
                zox_log("[tst] chunk_uvs: block [%s] drew no face", zox_getn(zox_tst_cuvs_block(world, zox_tst_cuvs_regular[i])));
                missing++;
            }
        }
        zox_log("[tst] [metric] chunk_uvs solid: [%i] regular blocks, [%i] faces, [%i] not a placed voxel's, [%i] with another's uvs, [%i] blocks drew none",
            zox_tst_cuvs_regular_count, faces, unplaced, wrong, missing);
        zox_tst_check(zox_tst_cuvs_regular_count > 1, "the realm has [%i] regular blocks", zox_tst_cuvs_regular_count);
        zox_tst_check(faces && !unplaced, "[%i] of the solid mesh's [%i] faces aren't a placed voxel's", unplaced, faces);
        zox_tst_check(!wrong, "[%i] of the solid mesh's [%i] faces have uvs not their block's for their direction", wrong, faces);
        zox_tst_check(!missing, "[%i] of [%i] regular blocks drew no face", missing, zox_tst_cuvs_regular_count);
    }
    // liquids: their own see through mesh and texture, uvs within it
    for (byte i = 1; i < 3; i++) {
        const entity mesh = zox_tst_cuvs_mesh(world, zox_tst_cuvs_materials[i]);
        const entity texture = zox_get_child_by_id(world, zox_tst_cuvs_block(world, zox_tst_cuvs_blocks[i]), zox_id(Texture));
        const MeshUVs* uvs = zox_get(mesh, MeshUVs);
        int outside = 0;
        for (int v = 0; v < uvs->length; v++) {
            const float2 uv = uvs->value[v];
            outside += uv.x < 0 || uv.y < 0 || uv.x > 1 || uv.y > 1;
        }
        const char* name = i == 1 ? "water" : "oil";
        zox_log("[tst] [metric] chunk_uvs %s: [%i] faces, see through [%i], its texture [%i], [%i] uvs outside it", name,
            uvs->length / 4, zox_has(mesh, TransparentMesh), zox_valid(texture) && zox_get_link(world, mesh, MeshTextureLink) == texture,
            outside);
        zox_tst_check(zox_has(mesh, TransparentMesh), "the %s mesh isn't see through", name);
        zox_tst_check(zox_valid(texture) && zox_get_link(world, mesh, MeshTextureLink) == texture,
            "the %s mesh doesn't draw the %s's texture", name, name);
        zox_tst_check(uvs->length >= 4 && !outside, "[%i] of the %s mesh's uvs are outside its texture", outside, name);
    }
}

void zox_tst_chunk_uvs_setup() {
    zox_chunks3_gpu_meshing = 0;
}
