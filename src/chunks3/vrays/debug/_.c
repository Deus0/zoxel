extern entity get_linked_character(ecs*, entity);

uint debug_ui_raycasting(
    ecs *world,
    const entity player,
    char *buffer,
    const uint size,
    uint index
) {

    if (!player) {
        index += snprintf(buffer + index, size - index, "! invalid player\n");
        return index;
    }

    const entity character = get_linked_character(world, player);
    if (!zox_valid(character)) {
        index += snprintf(buffer + index, size - index, "[%s] has no character\n", zox_get_name(player));
        return index;
    }

    zox_geter(character, RaycastVoxelData, data);
    index += snprintf(buffer + index, size - index, "Hit Result [%i]\n", data->result);
    index += snprintf(buffer + index, size - index, "   + positionl [%ix%ix%i]\n",
        data->positionl.x, data->positionl.y, data->positionl.z);
    index += snprintf(buffer + index, size - index, "   + positionv [%ix%ix%i]\n",
        data->positionv.x, data->positionv.y, data->positionv.z);
    index += snprintf(buffer + index, size - index, "   + positionf [%fx%fx%f]\n",
        data->positionf.x, data->positionf.y, data->positionf.z);
    index += snprintf(buffer + index, size - index, "   + voxel_scale [%f]\n",
        data->voxel_scale);

    return index;
}

/*typedef struct {
    byte result;
    byte voxel;
    entity hit_block;
    // current
    byte3 positionl;
    int3 positionv;
    float3 positionf;
    float voxel_scale;
    entity chunk;
    VoxelNode *node;
    float3 hit;
    float3 normal;
    float distance;
    // last
    entity chunk_last;
    VoxelNode *node_last;
    byte3 positionl_last;
    int3 positionv_last;
    float3 positionf_last;
} RaycastVoxelData;*/