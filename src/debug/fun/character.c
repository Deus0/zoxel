entity real_position_to_chunk(
    ecs* world,
    entity terrain,
    float3 position)
{
    zox_geter_value(terrain, BlockScale, float, terrain_scale);
    zox_geter_value(terrain, NodeDepth, byte, node_depth);
    short length = octree_size(node_depth);
    int3 chunk_position = real_position_to_chunk_position(position, length, terrain_scale);
    zox_geter(terrain, ChunkLinks, chunks);
    return int3_hashmap_get(chunks->value, chunk_position);
}


uint zox_dbg_label_character_links(
    ecs *world,
    entity player,
    char *buffer,
    uint size,
    uint index)
{
    entity e = zox_get_link(world, player, Character);
    if (!zox_valid(e)) {
        return index;
    }
    float3 position = zox_getv(e, Position3D);
    float3 euler = zox_getv(e, Euler);
    entity camera = zox_get_link(world, e, Camera);
    entity chunk = zox_getv(e, ChunkLink);
    entity tunk = zox_valid(chunk) ?
        zox_get_link(world, chunk, TunkLink) :
        0;
    entity region = zox_valid(tunk) ?
        zox_get_link(world, tunk, RegionLink) :
        0;
    // Character
    index += snprintf(buffer + index, size - index,
        "Character [%s]\n", zox_get_name(e));
    index += snprintf(buffer + index, size - index,
        " - Position [%.1fx%.1fx%.1f]\n", position.x, position.y, position.z);
    index += snprintf(buffer + index, size - index,
        " - Euler [%.1fx%.1fx%.1f]\n", euler.x, euler.y, euler.z);
    index += snprintf(buffer + index, size - index,
        " - Region [%s]\n", zox_getn(region));
    index += snprintf(buffer + index, size - index,
        " - Tunk [%s]\n", zox_getn(tunk));
    if (zox_valid(tunk)) {
        byte tunk_lod = zox_getv(tunk, TunkLod);
        index += snprintf(buffer + index, size - index,
            "   - TLod [%i]\n", tunk_lod);
    }
    index += snprintf(buffer + index, size - index,
        " - Chunk [%s]\n", zox_getn(chunk));
    if (zox_valid(camera)) {
        index += snprintf(buffer + index, size - index,
            "Camera [%s]\n", zox_getn(camera));
        zox_geter_value(camera, Position3D, float3, camera_position);
        zox_geter_value(camera, Euler, float3, camera_euler);
        index += snprintf(buffer + index, size - index,
            " - pos [%.1fx%.1fx%.1f]\n", camera_position.x, camera_position.y, camera_position.z);
        index += snprintf(buffer + index, size - index,
            " - eul [%.1fx%.1fx%.1f]\n", camera_euler.x, camera_euler.y, camera_euler.z);
    }
    return index;
}

uint zox_dbg_label_chunk_mesh(
    ecs* world,
    entity e,
    char* buffer,
    uint size,
    uint index)
{
    byte dbg_level = 1;
    // byte disabled = zox_is_disabled(e3);
    byte disabled = zox_has(e, Disabled);
    byte build_disabled = zox_has(e, BuildDisabled);
    byte depth = zox_getv(e, RenderDepth);
    byte visible = !zox_getv(e, RenderDisabled);
    byte build = zox_has(e, BuildMesh);
    byte build_colors = zox_has(e, BuildMeshColors);
    uint indicies_count = zox_get(e, MeshIndicies)->length;
    uint verts_count = zox_get(e, MeshVertices)->length;
    uint uvs_count = zox_get(e, MeshUVs)->length;
    uint colors_count = zox_get(e, MeshColorRGBs)->length;
    // GPU Links
    byte mesh_dirty = zox_has(e, MeshDirty);
    byte colors_dirty = zox_has(e, MeshColorsDirty);
    uint count = zox_getv(e, MeshRenderCount);
    guint2 mesh = zox_getv(e, MeshGPULink);
    guint uvs = zox_getv(e, UvsGPULink);
    guint colors = zox_getv(e, ColorsGPULink);
    index += snprintf(buffer + index, size - index,
        " + Mesh [%s]\n",
            zox_getn(e));
    index += snprintf(buffer + index, size - index,
        "   - Depth [%i] Disabled [%i]\n",
            depth,
            disabled);
    if (dbg_level < 2) {
        return index;
    }
    index += snprintf(buffer + index, size - index,
        "   - Visible [%i] Build Disabled [%i]\n",
        visible,
        build_disabled);
    index += snprintf(buffer + index, size - index,
        "   - Build Mesh [%i] Build Colors [%i]\n",
        build,
        build_colors);
    index += snprintf(buffer + index, size - index,
        "   - Mesh Dirty [%i] Colors Dirty [%i]\n",
        mesh_dirty,
        colors_dirty);
    index += snprintf(buffer + index, size - index,
        "   - x[%u] Ind [%i] Verts [%i] UVs [%i] Colors [%i]\n",
        count,
        indicies_count,
        verts_count,
        uvs_count,
        colors_count);
    index += snprintf(buffer + index, size - index,
        "   - Mesh [%ix%i] UVs [%i] Colors [%i]\n",
        mesh.x,
        mesh.y,
        uvs,
        colors);
    return index;
}

uint zox_dbg_label_inside_chunk(
    ecs *world,
    entity player,
    char *buffer,
    uint size,
    uint index)
{
    entity game = zox_get_parent(world, player);
    entity realm = zox_getv(game, RealmLink);
    if (!zox_valid(realm)) {
        return index;
    }
    entity terrain = zox_get_link(world, realm, Terrain);
    if (!zox_valid(terrain)) {
        return index;
    }
    entity camera = zox_get_link(world, player, Camera);
    if (!zox_valid(camera)) {
        return index;
    }
    float3 camera_position = zox_getv(camera, Position3D);
    entity chunk = real_position_to_chunk(
        world,
        terrain,
        camera_position);
    if (!zox_valid(chunk)) {
        index += snprintf(buffer + index, size - index,
            "Not Inside Chunk [%s]\n",
            zox_getn(chunk));
        return index;
    }
    entity tunk = zox_valid(chunk) ?
        zox_get_link(world, chunk, TunkLink) :
        0;
    entity region = zox_valid(tunk) ?
        zox_get_link(world, tunk, RegionLink) :
        0;
    index += snprintf(buffer + index, size - index,
        "Inside Chunk [%s]\n",
        zox_getn(chunk));
    index += snprintf(buffer + index, size - index,
        " - Tunk [%s]\n", zox_getn(tunk));
    if (zox_valid(tunk)) {
        byte tunk_lod = zox_getv(tunk, TunkLod);
        byte generate2 = zox_getv(tunk, GenerateTunk);
        index += snprintf(buffer + index, size - index,
            "   - Lod [%i]\n", tunk_lod);
        index += snprintf(buffer + index, size - index,
            "   - Generate [%i]\n",
            generate2);
    }
    index += snprintf(buffer + index, size - index,
        " - Region [%s]\n",
        zox_get_name(region));
    int3 position = zox_getv(chunk, ChunkPosition);
    byte depth = zox_getv(chunk, NodeDepth);
    byte render_depth = zox_getv(chunk, RenderDepth);
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    const SidesOctree* sides = zox_get(chunk, SidesOctree);
    byte lod_dirty = zox_has(chunk, ChunkLodDirty) &&
        zox_getv(chunk, ChunkLodDirty);
    byte voxels_dirty = zox_has(chunk, VoxelNodeDirty);
    byte build = zox_has(chunk, BuildChunkSides);
    index += snprintf(buffer + index, size - index,
        " - At [%ix%ix%i]\n",
            position.x,
            position.y,
            position.z);
    index += snprintf(buffer + index, size - index,
        " - Octree Depth [%i] Render Depth [%i]\n",
            depth,
            render_depth);
    index += snprintf(buffer + index, size - index,
        " - Voxels [%i] Sides [%i]\n",
            voxels->value,
            sides->value);
    index += snprintf(buffer + index, size - index,
        " - Lod Dirty [%i]\n",
            lod_dirty);
    index += snprintf(buffer + index, size - index,
        " - Voxels Dirty [%i]\n",
            voxels_dirty);
    index += snprintf(buffer + index, size - index,
        " - Build Sides [%i]\n",
            build);
    if (zox_has(chunk, GenerateChunk)) {
        byte generate = zox_getv(chunk, GenerateChunk);
        index += snprintf(buffer + index, size - index,
            " - generate [%i]\n",
                generate);
    }
    // Chunk Meshes
    // entity active_mesh = zox_getv(chunk, ActiveMesh);
    // entity preparing_mesh = zox_getv(chunk, PreparingMesh);
    entity active_mesh = zox_get_link(world, chunk, ActiveMesh);
    entity preparing_mesh = zox_get_link(world, chunk, PreparingMesh);
    index += snprintf(buffer + index, size - index,
        "+ Active [%s]\n",
        zox_getn(active_mesh));
    index += snprintf(buffer + index, size - index,
        "- Preparing [%s]\n",
        zox_getn(preparing_mesh));
    entity meshes[8];
    uint meshes_length = zox_get_children_by_id(world, chunk, meshes, 8, zox_id(ChunkMesh));
    index += snprintf(buffer + index, size - index,
        "Meshes [%i]\n",
        meshes_length);
    for (int k = 0; k < meshes_length; k++) {
        entity e3 = meshes[k];
        index = zox_dbg_label_chunk_mesh(
            world,
            e3,
            buffer,
            size,
            index);
    }
    return index;
}

uint zox_dbg_label_towns(
    ecs *world,
    entity player,
    char *buffer,
    uint size,
    uint index)
{
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        return index;
    }
    entity chunk = zox_getv(character, ChunkLink);
    if (!zox_valid(chunk)) {
        return index;
    }
    entity tunk = zox_valid(chunk) ?
        zox_get_link(world, chunk, TunkLink) :
        0;
    entity region = zox_valid(tunk) ?
        zox_get_link(world, tunk, RegionLink) :
        0;
    entity terrain = zox_get_parent(world, chunk);
    // Character
    float3 position = zox_getv(character, Position3D);
    float terrain_scale = zox_getv(terrain, BlockScale);
    int3 block_position = real_position_to_block_position(position, terrain_scale);
    index += snprintf(buffer + index, size - index,
        " - Position [%.1fx%.1fx%.1f]\n",
        position.x, position.y, position.z);
    index += snprintf(buffer + index, size - index,
        " - Block Position [%ix%ix%i]\n",
        block_position.x, block_position.y, block_position.z);
    index += snprintf(buffer + index, size - index,
        "Region [%s]\n",
        zox_get_name(region));
    if (!zox_valid(region)) {
        return index;
    }
    entity towns[zox_children_capacity];
    uint towns_length = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
    index += snprintf(buffer + index, size - index,
        "Total Towns [%i]\n",
        towns_length);
    for (uint i = 0; i < towns_length; i++) {
        entity town = towns[i];
        zox_geter(town, ZoxName, name);
        lint seed = zox_getv(town, Seed);
        int2 position = zox_getv(town, BlockPosition2);
        byte2 size2 = zox_getv(town, TownSize);
        index += snprintf(buffer + index, size - index,
            " Town [%s]\n", zox_get_name(town));
        index += snprintf(buffer + index, size - index,
            " - Name [%s]\n", name->value);
        index += snprintf(buffer + index, size - index,
            " - Seed [%lu]\n", seed);
        index += snprintf(buffer + index, size - index,
            " - Position [%ix%i]\n", position.x, position.y);
        index += snprintf(buffer + index, size - index,
            " - Size [%ix%i]\n", size2.x, size2.y);
        // TODO: Get Homes list too
        // zox_set(e, WallHeight, { wall_height });
        // zox_set(e, WallThickness, { wall_thickness });
    }
    return index;
}
