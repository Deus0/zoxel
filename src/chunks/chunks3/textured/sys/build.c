// Doing: Keeping in one place, making iteration easier
// TODO: Remove structs!!!
// todo: keep this in hashmap for processed Voxes - atm its gonna check 10k chunks * voxels length which is bad
// todo: support for multiple terrains using hashmap

byte zox_disable_node_face_subdivision = 1;
// remember: vertex position is just node position / voxel position

// static data
typedef struct {
    // chunk data
    const VoxelNode* root;
    const byte rdepth;
    const mesh_uvs_build_data* mesh_data;
    byte edge_voxel;
    // voxels
    const TilemapUVs* tilemap_uvs;
    const byte* voxel_solidity;
} terrain_build_data;

// changing data
typedef struct {
    // node data
    const VoxelNode *parent;
    const VoxelNode *node;
    byte depth;
    int3 position;
    float scale;
    byte index;
    byte voxel;
    // face data
    byte direction;
    float3 positionf;
    byte3 local_position;
} octree_dig_data;

// this takes 14ms on a 24core cpu, 6ms though during streaming
// scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset
void zox_build_voxel_face(const mesh_uvs_build_data* mesh, const int* indicies, const float3* verts, const float2* uvs, float3 position, float3 scale3) {
    if (!uvs) { // TODO: If we dont generate realm blocks  it goes out of index, we should account for this
        return;
    }
    // indicies
    expand_capacity_int_array_d(mesh->indicies, voxel_face_indicies_length);
    for (byte i = 0; i < 6; i++) {
        int j = mesh->indicies->size + i;
        mesh->indicies->data[j] = mesh->vertices->size + indicies[i];
    }
    mesh->indicies->size += voxel_face_indicies_length;
    // verts
    expand_capacity_float3_array_d(mesh->vertices, voxel_face_vertices_length);
    for (byte i = 0; i < voxel_face_vertices_length; i++) {
        int j = mesh->vertices->size + i;
        float3 vert = verts[i];
        float3_scale3p(&vert, scale3);
        float3_add_float3_p(&vert, position);
        mesh->vertices->data[j] = vert;
    }
    mesh->vertices->size += voxel_face_vertices_length;
    // uvs
    expand_capacity_float2_array_d(mesh->uvs, voxel_face_vertices_length);
    for (byte i = 0; i < 4; i++) {
        int j = mesh->uvs->size + i;
        mesh->uvs->data[j] = uvs[i];
    }
    mesh->uvs->size += voxel_face_vertices_length;
    // White for now, alter in other systems
    for (byte a = 0; a < voxel_face_vertices_length; a++) {
        add_to_color_rgb_array_d(mesh->color_rgbs, color_rgb_white);
    }
}


// NOTE: A special case here if neighbor is lesser / higher
// NOTE: For some reason I had to add 1 here, but is fine in other system, the main diff is the dig depth vs render depth
// What we really need is adjacent node depth vs dig depth, not render depth differences
// actually this makes sense: we are just checking what neighbor is rendering at verse what we are
static inline void zox_terrain_building_dig(terrain_build_data data, octree_dig_data dig, const SidesOctree* sides) {
    // Dig Deeper
    if (dig.depth < data.rdepth && sides->ptr) { // !is_closed_SidesOctree(sides)) {
        const SidesOctree* sides_kids = (const SidesOctree*) sides->ptr;
        // get_children_SidesOctree(sides);
        byte has_vkids = !is_closed_VoxelNode(dig.node);
        const VoxelNode* vkids = has_vkids ? get_children_VoxelNode(dig.node) : NULL;
        byte child_depth = dig.depth + 1;
        float child_scale = dig.scale * 0.5f;
        int3 position = dig.position;
        int3_multiply_int_p(&position, 2);
        for (byte i = 0; i < 8; i++) {
            const VoxelNode* cvoctree = has_vkids ? &vkids[i] : dig.node;
            int3 cposition = int3_add(position, octree_positions[i]);
            octree_dig_data child = {
                .parent = dig.node,
                .depth = child_depth,
                .scale = child_scale,
                .node = cvoctree,
                .index = i,
                .position = cposition
            };
            zox_terrain_building_dig(data, child, &sides_kids[i]);
        }
        return;
    }
    if (!sides->value) {
        return;
    }
    if (!dig.node->value) {
        zox_log_error("Sides error, air cannot render.");
        return;
    }
    dig.voxel = dig.node->value;
    dig.positionf = float3_from_int3(dig.position);
    float3_scale_p(&dig.positionf, dig.scale);
    dig.local_position = octree_positions_b[dig.index];
    // NOTE: UVIndex is calculated by block index * 6 for faces
    byte block_index = dig.voxel - 1;
    uint block_uv_index = block_index * 6 * 4;
    byte dbg_log = 0;
    if (dbg_log && block_index == 0) {
        zox_log("-b- Block [%i], UV Index [%i]", block_index, block_uv_index);
    }
    for (dig.direction = 0; dig.direction < 6; dig.direction++) {
        if (!(sides->value & (1 << (dig.direction + 1)))) {
            continue;
        }
        // uint face_uv_index = data.voxel_uv_indexes[block_face_index + dig.direction];
        uint face_uv_index = block_uv_index + dig.direction * 4;
        octree_face_data face = {
            .indicies = voxel_face_indicies_n + dig.direction * voxel_face_indicies_length,
            .vertices = voxel_face_vertices_n[dig.direction],
            .uvs = &data.tilemap_uvs->value[face_uv_index],
        };
        zox_build_voxel_face(data.mesh_data, face.indicies, face.vertices, face.uvs, dig.positionf, float3_single(dig.scale));
    }
}

// TODO: Move terrain cache into functions
// TODO: Cache all managers found, not just single
// NOTE: Rebuilds Chunk when ChunkMeshDirty is dirty
zox_sys2(Chunk3TexturedBuildSystem) {
    zox_sys_world();
    zox_sys_begin_at(1);
    zox_sys_in(TilemapLink);
    zox_sys_in(ChunkMeshDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(BlockScale);
    zox_sys_in(VoxelNode);
    zox_sys_in(SidesOctree);
    zox_sys_out(MeshIndicies);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshUVs);
    zox_sys_out(MeshColorRGBs);
    zox_sys_out(MeshDirty);
    // Does a sweep of states first
    byte any_dirty = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ChunkMeshDirty, cdirty)
        if (cdirty->value == zox_dirty_active) {
            any_dirty = 1;
            break;
        }
    }
    if (!any_dirty) {
        return;
    }
    chunk3_textured_builder_data build_data;
    if (!cache_blocks_data(it, &build_data)) {
        return;
    }
    // Our Loop
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TilemapLink, tilemap);
        zox_sys_i(ChunkMeshDirty, chunk_mesh_dirty);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(BlockScale, bscale);
        zox_sys_i(VoxelNode, voctree);
        zox_sys_i(SidesOctree, sides);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, verts);
        zox_sys_o(MeshColorRGBs, colors);
        zox_sys_o(MeshUVs, uvs);
        zox_sys_o(MeshDirty, mesh_dirty);
        if (chunk_mesh_dirty->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(tilemap->value) || !zox_has(tilemap->value, TilemapUVs)) {
            zox_sys_e();
            zox_loge("Tilemap not found on Chunk Terrain [%s]", zox_get_name(e));
            continue;
        }
        zox_geter(tilemap->value, TilemapUVs, tilemap_uvs);
        if (!tilemap_uvs->value || !tilemap_uvs->length) {
            zox_sys_e();
            zox_loge("Tilemap has no TilemapUVs on Chunk Terrain [%s]", zox_get_name(e));
            continue;
        }
        // No Mesh Sides were found
        if (!sides->value) {
            clear_mesh_uvs(indicies, verts, colors, uvs);
            mesh_dirty->value = mesh_state_trigger_terrain;
            continue;
        }
        byte vlength = powers_of_two[rdepth->value];
        float cscale = bscale->value * vlength;
        mesh_uvs_build_data mesh_data = {
            .indicies = create_int_array_d(initial_dynamic_array_size),
            .vertices = create_float3_array_d(initial_dynamic_array_size),
            .uvs = create_float2_array_d(initial_dynamic_array_size),
            .color_rgbs = create_color_rgb_array_d(initial_dynamic_array_size)
        };
        // build out mesh data
        terrain_build_data data = {
            .tilemap_uvs = tilemap_uvs,
            .voxel_solidity = build_data.solidity,
            .mesh_data = &mesh_data,
            .root = voctree,
            .rdepth = rdepth->value,
        };
        octree_dig_data dig = {
            .parent = NULL,
            .node = voctree,
            .scale = cscale,
        };
        read_lock_VoxelNode(voctree);
        zox_terrain_building_dig(data, dig, sides);
        read_unlock_VoxelNode(voctree);
        // sizes
        indicies->length = mesh_data.indicies->size;
        verts->length = mesh_data.vertices->size;
        uvs->length = mesh_data.uvs->size;
        colors->length = mesh_data.color_rgbs->size;
        // data
        indicies->value = zinalize_int_array_d(mesh_data.indicies);
        verts->value = zinalize_float3_array_d(mesh_data.vertices);
        colors->value = zinalize_color_rgb_array_d(mesh_data.color_rgbs);
        uvs->value = zinalize_float2_array_d(mesh_data.uvs);
        // dirty
        mesh_dirty->value = mesh_state_trigger_terrain;
        // mesh_dirty->value = mesh_state_trigger;
        // zox_log("Building Terrain Chunk! Verts [%i] Scale [%f] Depth [%i]", verts->length, cscale, rdepth);
    }
    free_chunk3_textured_builder_data(build_data);
} zox_sys_end(Chunk3TexturedBuildSystem);
