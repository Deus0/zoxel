// NOTE: Uses terrainMap to spawn terrain blocks in chunks
// TODO: Use a general Generate state instead of RenderDepthDirty
zox_sys2(TownWallsSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxLink);
    zox_sys_in(TunkLink);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, state);
        zox_sys_i(NodeDepth, vdepth);
        zox_sys_i(ChunkPosition, cposition);
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(TunkLink, tunk);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(VoxelNodeDirty, dirty);
        if (state->value != zox_dirty_active) {
            continue;
        }
        zox_geter_value(terrain->value, NodeDepth, byte, terrain_depth);
        entity realm = zox_getv(terrain->value, RealmLink);
        byte is_max_depth = vdepth->value == terrain_depth;
        if (!is_max_depth) {
            continue;
        }
        entity wall = zox_get_child_by_id(world, realm, zox_id(BlockBricks));
        if (!zox_valid(wall)) {
            zox_loge("No wall for town..");
            continue;
        }
        byte bricks_id = zox_getv(wall, BlockIndex);
        if (!bricks_id) {
            continue;
        }
        byte voctree_length = powers_of_two_byte[vdepth->value];
        int3 chunk_voxel_position = (int3) { cposition->value.x * voctree_length, cposition->value.y * voctree_length, cposition->value.z * voctree_length };
        int chunk_position_y = cposition->value.y * voctree_length;
        byte3 positionl;
        int hmultiplier = 1;
        byte ccc = vdepth->value;
        while (ccc != terrain_depth) {
            hmultiplier *= 2;
            ccc++;
        }
        int max_chunk_length = powers_of_two[terrain_depth];
        int2 hsize = int2_single(max_chunk_length);
        if (!zox_valid(tunk->value)) {
            zox_log_error("Invalid [Tunk] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        zox_geter(tunk->value, HeightMap, hmap);
        zox_geter(tunk->value, TownMap, tmap);
        if (!tmap->length) {
            zox_log_error("Invalid [Tunk] [Maps] at [%ix%ix%i]", cposition->value.x, cposition->value.y, cposition->value.z);
            continue;
        }
        write_lock_VoxelNode(voctree);
        for (positionl.x = 0; positionl.x < voctree_length; positionl.x++) {
            for (positionl.z = 0; positionl.z < voctree_length; positionl.z++) {
                int2 hposition = (int2) {
                    positionl.x * hmultiplier,
                    positionl.z * hmultiplier
                };
                int hindex = int2_array_index(hposition, hsize);
                int global_position_y = (int) (hmap->value[hindex]);
                int local_height_raw = global_position_y - chunk_position_y;
                byte town = tmap->value[hindex];
                if (town) {
                    if (dbg_log) {
                        zox_log("Placing Town Wall at [%ix%i]", chunk_voxel_position.x + positionl.x, chunk_voxel_position.z + positionl.z);
                    }
                    for (int h = 1; h <= 4; h++) {
                        positionl.y = local_height_raw + h;
                        if (positionl.y >= 0 && positionl.y < voctree_length) {
                            set_clean_VoxelNode(voctree, vdepth->value, positionl, bricks_id);
                        }
                    }
                }
            }
        }
        write_unlock_VoxelNode(voctree);
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(TownWallsSystem);
