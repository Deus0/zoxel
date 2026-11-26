// Doing: Keeping in one place, making iteration easier
// TODO: Remove structs!!!
// todo: keep this in hashmap for processed Voxes - atm its gonna check 10k chunks * voxels length which is bad
// todo: support for multiple terrains using hashmap

#ifdef zox_tests
static void capture_test_mesh_border(ecs* world, entity e, const VoxelNode* voxels, VoxelNodeLock* voxels_lock, byte depth) {
    // Store the voxels used by this build. The live tree may change while its old mesh still draws.
    TestMeshBorder* border = zox_get_mut(e, TestMeshBorder);
    const int length = 1 << depth;
    resize_TestMeshBorder(border, 6 * length * length);
    if (border->length != 6 * length * length) {
        return;
    }
    spin_lock(&voxels_lock->value);
    for (int d = 0; d < 6; d++) {
        const int axis = d / 2, a = axis == 0 ? 1 : 0, b = axis == 2 ? 1 : 2;
        int p[3];
        p[axis] = d % 2 ? length - 1 : 0;
        for (int u = 0; u < length; u++)
        for (int v = 0; v < length; v++) {
            p[a] = u;
            p[b] = v;
            border->value[(d * length + u) * length + v] =
                getv_VoxelNode(voxels, depth, (byte3) { p[0], p[1], p[2] });
        }
    }
    spin_unlock(&voxels_lock->value);
}
#endif

byte zox_disable_node_face_subdivision = 1;
extern byte zox_no_lights;
// remember: vertex position is just node position / voxel position

typedef struct {
    int_array_d *indicies;
    float3_array_d* vertices;
    float2_array_d* uvs;
    color_array_d* color_rgbs;
} mesh_uvs_build_data;


// static data
typedef struct {
    const mesh_uvs_build_data* mesh_data;
    const TilemapUVs* tilemap_uvs;
    const byte* voxel_solidity;
    const byte* heights;    // a block's height in eighths, by block index - 1 (BlockWaterLevel: flowing water), 0 full
    byte whole_texture;     // a liquid's mesh: each face its own texture's whole, not a tilemap tile
} terrain_build_data;

// a face with its uvs (a tilemap tile's 4, or a texture's whole), white till smooth lights colour it
static inline void zox_build_voxel_face(
    const mesh_uvs_build_data* mesh,
    const int* face_indicies,
    const float3* face_verts,
    const float2* face_uvs,
    float3 position,
    float3 scale)
{
    mesh_add_face(mesh->indicies, mesh->vertices, face_indicies, face_verts, position, scale.x);
    for (byte i = 0; i < 4; i++) {
        float2_array_d_add(mesh->uvs, face_uvs[i]);
        color_array_d_add(mesh->color_rgbs, color_glow(color_rgb_white, 0));
    }
}


#include "build_grid.c"

// TODO: Cache Multiple Voxel Managers, not just single
// NOTE: Rebuilds Chunk when BuildChunkMesh is dirty
void chunk_textured_build_system(iter* it) {
    byte dbg_log = 0;
    byte max_process = !zox_disable_process_skips ? 1 : 0;
    entity cached_blocks_parent = 0;
    byte solidity[255];
    memset(solidity, 1, 255);
    byte heights[255];
    memset(heights, 0, 255);
    entity tilemap_cache = 0;
    const TilemapUVs* tilemap_uvs = NULL;
    // grid meshing: the block materials table, a material's drawn and hiding voxels, the grid's buffer, this run's
    byte block_materials[256];
    byte grid_drawn[256], grid_hides[256];
    entity grid_manager = 0;
    byte grid_material = 255;
    byte* grid_buffer = NULL;
    size_t grid_buffer_size = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(MeshIndicies);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshUVs);
    zox_sys_out(MeshColors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, verts);
        zox_sys_o(MeshColors, colors);
        zox_sys_o(MeshUVs, uvs);
        // NOTE: Delay if past limit [max_process]
        if (max_process && process_count > max_process) {
            break;
        }
        // Get chunk data: the mesh's material holds its sides, its chunk the voxels
        entity material = zox_get_parent(world, e);
        entity chunk = zox_valid(material) ? zox_get_parent(world, material) : 0;
#ifdef zox_safety_checks
        if (!zox_valid(chunk)) {
            zox_loge("Chunk Parent Invalid for [%s]", zox_sys_e_name);
            continue;
        }
        if (!zox_has(chunk, NodeDepth) ||
            !zox_has(chunk, VoxelNode) ||
            !zox_has(chunk, VoxelNodeLock)
        ) {
            zox_loge("[build_textured] Chunk Components Invalid [%s]", zox_sys_e_name);
            continue;
        }
#endif
        // Chunk Validation
        byte chunk_depth = zox_getv(chunk, NodeDepth);
        if (depth->value > chunk_depth) {
            zox_loge("Chunk Mesh Depth > Chunks [%s] [%i] > [%i]",
                zox_getn(chunk),
                depth->value,
                chunk_depth);
            continue;
        }
        entity terrain = zox_get_parent(world, chunk);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_logw("Terrain Invalid for [%s]",
                zox_getn(chunk));
            // continue;
        } else if (!zox_has(terrain, NodeDepth) ||
            !zox_has(terrain, BlockScale))
        {
            zox_loge("Terrain Invalid Components [%s]",
                zox_getn(chunk));
            continue;
        }
#endif
        byte terrain_depth;
        float terrain_scale;
        if (zox_valid(terrain)) {
            terrain_depth = zox_getv(terrain, NodeDepth);
            terrain_scale = zox_getv(terrain, BlockScale);
        } else {
            terrain_depth = chunk_depth;
            terrain_scale = zox_getv(chunk, BlockScale);
        }
        entity blocks_parent = zox_get_link(world, chunk, BlockManagerLink);
#ifdef zox_safety_checks
        if (!zox_valid(blocks_parent)) {
            zox_loge("Blocks Manager Invalid for [%s]", zox_getn(chunk));
            continue;
        }
#endif
        if (blocks_parent != cached_blocks_parent) {
#ifdef zox_safety_checks
            if (!blocks_parent) {
                zox_loge("Failed to find BlockManagerLink on Chunk from ChunkMeshes");
                continue;
            }
#endif
            const BlockLinks* blocks = zox_get(blocks_parent, BlockLinks);
#ifdef zox_safety_checks
            if (!blocks->length) {
                continue; // if failed to find terrain parents
            }
#endif
            cached_blocks_parent = blocks_parent;
            memset(solidity, 1, blocks->length);
            memset(heights, 0, 255);
            for (int j = 0; j < blocks->length && j < 255; j++) {
                entity block = blocks->value[j];
                if (zox_valid(block) && zox_has(block, BlockWaterLevel) && zox_getv(block, BlockWaterLevel) < zox_water_level_source) {
                    heights[j] = zox_getv(block, BlockWaterLevel);
                }
                if (!zox_valid(block) ||
                    !zox_has(block, BlockModel))
                {
                    continue;
                }
                solidity[j] =
                    zox_getv(block, BlockModel) == zox_block_solid;
            }
        }
        entity tilemap = zox_get_link(world, chunk, Tilemap);
        if (!zox_valid(tilemap)) {
            continue;
        }
        if (tilemap_cache != tilemap) {
#ifdef zox_safety_checks
            if (!zox_valid(tilemap) ||
                !zox_has(tilemap, TilemapUVs) ||
                !zox_has(tilemap, GenerateTexture))
            {
                zox_loge("Tilemap not found on Chunk Terrain [%s]", zox_sys_e_name);
                continue;
            }
#endif
            if (zox_getv(tilemap, GenerateTexture)) {
                if  (dbg_log) {
                    zox_log("Tilemap Still Generating [%s]: %i",
                        zox_getn(tilemap),
                        zox_getv(tilemap, GenerateTexture));
                }
                continue;
            }
            tilemap_uvs = zox_get(tilemap, TilemapUVs);
#ifdef zox_safety_checks
            if (!tilemap_uvs->value || !tilemap_uvs->length) {
                zox_loge("Tilemap TilemapUVs on Chunk Terrain [%s] has not generated", zox_get_name(e));
                continue;
            }
#endif
            tilemap_cache = tilemap;
        }
        const VoxelNode* voxels = zox_get(chunk, VoxelNode);
        VoxelNodeLock* voxels_lock = zox_mut(chunk, VoxelNodeLock);
        float block_scale = get_chunk_scale(
            depth->value,
            terrain_depth,
            terrain_scale);
        short length = octree_size(depth->value);
        float chunk_scale = block_scale * length;
        mesh_uvs_build_data mesh_data = {
            .indicies = create_int_array_d(initial_dynamic_array_size),
            .vertices = create_float3_array_d(initial_dynamic_array_size),
            .uvs = create_float2_array_d(initial_dynamic_array_size),
            .color_rgbs = create_color_array_d(initial_dynamic_array_size)
        };
        // build out mesh data
        terrain_build_data data = {
            .tilemap_uvs = tilemap_uvs,
            .whole_texture = zox_has(material, BlockMaterial) && block_material_liquid(zox_getv(material, BlockMaterial)),
            .voxel_solidity = solidity,
            .heights = heights,
            .mesh_data = &mesh_data,
        };
        const byte block_material = zox_has(material, BlockMaterial) ? zox_getv(material, BlockMaterial) : zox_block_material_solid;
        if (grid_manager != blocks_parent || grid_material != block_material) {
            if (grid_manager != blocks_parent) {
                get_block_materials(world, blocks_parent, block_materials);
            }
            grid_manager = blocks_parent;
            grid_material = block_material;
            voxel_grid_material(block_materials, block_material, grid_drawn, grid_hides);
        }
        grid_buffer = voxel_grid_buffer(grid_buffer, &grid_buffer_size, depth->value);
        if (grid_buffer) {
            voxel_grid grid;
            chunk_voxel_grid(world, chunk, depth->value, grid_hides, grid_buffer, &grid);
            build_textured_grid(&grid, data, depth->value, chunk_scale, grid_drawn, grid_hides);
        }
#ifdef zox_tests
        if (zox_test_capture_mesh_border && zox_has(e, TestMeshBorder)) {
            capture_test_mesh_border(world, e, voxels, voxels_lock, depth->value);
        }
#endif
        // Set Entity data from Dynamic Arrays, the last build's freed first (they leaked ~100mb a game, chunks rebuild
        //  as their lods and lights change: tst end_game_cleanup's heap)
        dispose_MeshIndicies(indicies);
        dispose_MeshVertices(verts);
        dispose_MeshUVs(uvs);
        dispose_MeshColors(colors);
        indicies->length = mesh_data.indicies->size;
        verts->length = mesh_data.vertices->size;
        uvs->length = mesh_data.uvs->size;
        colors->length = mesh_data.color_rgbs->size;
        indicies->value = finalize_arrayd_int(mesh_data.indicies);
        verts->value = finalize_arrayd_float3(mesh_data.vertices);
        colors->value = finalize_arrayd_color(mesh_data.color_rgbs);
        uvs->value = finalize_arrayd_float2(mesh_data.uvs);
        // dirty
        zox_remove(e, BuildMesh);
        zox_add(e, MeshDirty);
        if (!zox_no_lights) {
            zox_add(e, BuildMeshColors);
        } /*else {
            zox_add(e, MeshDirty);
        }*/
        if (dbg_log) {
            zox_log("Built Mesh [%s]:[%s] Verts [%i] Scale [%f] Depth [%i]",
                zox_getn(e),
                zox_getn(chunk),
                verts->length,
                chunk_scale,
                depth->value);
        }
        zox_sys_increment();
    }
    free(grid_buffer);
    zox_sys_on_end();
} zoxd_system(chunk_textured_build_system);
