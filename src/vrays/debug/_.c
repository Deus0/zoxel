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
    index += snprintf(buffer + index, size - index, "Raycaster Debugger [%i]\n", data->result);
    index += snprintf(buffer + index, size - index, "   + dist [%f]\n", data->distance);
    index += snprintf(buffer + index, size - index, "   + hit [%fx%fx%f]\n",
        data->hit.x, data->hit.y, data->hit.z);
    index += snprintf(buffer + index, size - index, "   + normal [%fx%fx%f]\n",
        data->normal.x, data->normal.y, data->normal.z);

    index += snprintf(buffer + index, size - index, "   + positionl [%ix%ix%i]\n",
        data->positionl.x, data->positionl.y, data->positionl.z);
    index += snprintf(buffer + index, size - index, "   + positionl[L] [%ix%ix%i]\n",
        data->positionl_last.x, data->positionl_last.y, data->positionl_last.z);

    index += snprintf(buffer + index, size - index, "   + positionv [%ix%ix%i]\n",
        data->positionv.x, data->positionv.y, data->positionv.z);
    index += snprintf(buffer + index, size - index, "   + positionv[L] [%ix%ix%i]\n",
        data->positionv_last.x, data->positionv_last.y, data->positionv_last.z);

    index += snprintf(buffer + index, size - index, "   + positionf [%fx%fx%f]\n",
        data->positionf.x, data->positionf.y, data->positionf.z);

    index += snprintf(buffer + index, size - index, "   + voxel_scale [%f]\n",
        data->voxel_scale);


    // index += snprintf(buffer + index, size - index, "   * sides [%i]\n", data->node->sides);

    // node debugger
    if (data->node) {
        index += snprintf(buffer + index, size - index, "   * sides: 0x%02X [", data->node->sides);

        bool first = true;
        for (int face = 0; face < 6; face++) {
            if (data->node->sides & (1 << face)) {
                if (!first) {
                    index += snprintf(buffer + index, size - index, ", ");
                }
                index += snprintf(buffer + index, size - index, "%s", direction_names[face]);
                first = false;
            }
        }

        index += snprintf(buffer + index, size - index, "]\n");

        zox_geter_value(data->chunk, RenderDepth, byte, depth);
        zox_geter(data->chunk, LightNode, light_node);

        byte light = get_LightNode_value_ex(light_node, depth, data->positionl, 0);
        index += snprintf(buffer + index, size - index, "   + light [%i]\n", light);

        byte light_last = get_LightNode_value_ex(light_node, depth, data->positionl_last, 0);
        index += snprintf(buffer + index, size - index, "   + light_last [%i]\n", light_last);

        zox_geter(data->chunk, ChunkNeighbors, neighbors);
        const LightNode* nnodesl[6];
        fetch_neightbor_light_nodes(
            world,
            neighbors,
            nnodesl);
        if (!nnodesl[0]) {
            zox_log_error("Null Neighbor, weirdness: %s", zox_get_name(neighbors->value[0]));
        }
        byte face = normal_to_direction(data->normal);
        const LightNode* adj_node = get_LightNode_neighbor(
            light_node,
            nnodesl,
            face,
            data->positionl,
            depth);
        if (adj_node) {
            index += snprintf(buffer + index, size - index, "   + Adj Light [%i]\n", adj_node->value);
        } else {
            index += snprintf(buffer + index, size - index, "   - No Adj Node [%ix%ix%i] dir [%i]\n", data->positionl.x, data->positionl.y, data->positionl.z, face);
        }

    } else {
        index += snprintf(buffer + index, size - index, "   - No Voxel Node\n");
    }


    return index;
}