uint debug_ui_raycasted_light(
    ecs *world,
    entity player,
    char *buffer,
    uint size,
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
    index += snprintf(buffer + index, size - index, "Light Debugger [%i]\n", data->result);

    // node debugger
    if (data->node) {

        entity chunk_last = data->chunk_last;

        if (zox_valid(chunk_last)) {
            zox_geter_value(chunk_last, RenderDepth, byte, depth_last);
            zox_geter(chunk_last, LightNode, light_node_last);
            byte light_last = get_value_LightNode(light_node_last, depth_last, data->positionl_last, 0);
            index += snprintf(buffer + index, size - index, "   + Light Air [%i]\n", light_last);
            if (depth_last) {
                byte light_air_upper = get_value_LightNode(light_node_last, depth_last - 1, data->positionl_last, 0);
                index += snprintf(buffer + index, size - index, "   + Light Air (+1 depth) [%i]\n", light_air_upper);
            }

            if (zox_valid(data->chunk) && zox_has(data->chunk, ChunkNeighbors)) {
                zox_geter(data->chunk, ChunkNeighbors, neighbors);
                const LightNode* nnodesl[6];
                fetch_neightbor_light_nodes(
                    world,
                    neighbors,
                    nnodesl);
                if (!nnodesl[0]) {
                    entity n = neighbors->value[0];
                    zox_log_error("Null Neighbor, weirdness: %s",
                        zox_valid(n) ? zox_get_name(n) : "invalid");
                }
                // later we can get all lights nearby
                byte face = normal_to_direction(data->normal);
                const LightNode* adj_node = get_neighbor_LightNode(
                    light_node_last,
                    nnodesl,
                    face,
                    data->positionl,
                    depth_last
                );
                if (adj_node) {
                    index += snprintf(buffer + index, size - index, "   + Light [%i] [adj_check]\n", adj_node->value);
                } else {
                    index += snprintf(buffer + index, size - index, "   - No Adj Node [%ix%ix%i] dir [%i]\n", data->positionl.x, data->positionl.y, data->positionl.z, face);
                }
            } else {
                index += snprintf(buffer + index, size - index, "   - No Neighbors\n");
            }
        } else {
            index += snprintf(buffer + index, size - index, "   ! chunk_last invalid\n");
            // return index;
        }

        entity chunk_hit = data->chunk;
        zox_geter_value(chunk_hit, RenderDepth, byte, depth_hit);
        zox_geter(chunk_hit, LightNode, light_node_hit);
        byte light_hit = get_value_LightNode(light_node_hit, depth_hit, data->positionl, 0);
        index += snprintf(buffer + index, size - index, "   + Light Inside [%i]\n", light_hit);

        if (depth_hit) {
            byte light_hit_upper = get_value_LightNode(light_node_hit, depth_hit - 1, data->positionl, 0);
            index += snprintf(buffer + index, size - index, "   + Light Inside (+1 depth) [%i]\n", light_hit_upper);
        }

        index += snprintf(buffer + index, size - index, "   + positionl[L] [%ix%ix%i]\n",
            data->positionl_last.x,
            data->positionl_last.y,
            data->positionl_last.z);

        index += snprintf(buffer + index, size - index, "   + positionv[L] [%ix%ix%i]\n",
            data->positionv_last.x, data->positionv_last.y, data->positionv_last.z);

        // index += snprintf(buffer + index, size - index, "   * sides: 0x%02X [", data->node->sides);

        /*bool first = true;
        for (int face = 0; face < 6; face++) {
            if (data->node->sides & (1 << face)) {
                if (!first) {
                    index += snprintf(buffer + index, size - index, ", ");
                }
                index += snprintf(buffer + index, size - index, "%s", direction_names[face]);
                first = false;
            }
        }*/

        index += snprintf(buffer + index, size - index, "]\n");

        byte light = get_value_LightNode(light_node_hit, depth_hit, data->positionl, 0);
        index += snprintf(buffer + index, size - index, "   + inside light [%i]\n", light);

    } else {
        index += snprintf(buffer + index, size - index, "   - No Voxel Node\n");
    }


    return index;
}