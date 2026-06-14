uint debug_ui_raycasted_light(ecs *world, entity player, char *buffer, uint size, uint index) {
    if (!player) {
        index += snprintf(buffer + index, size - index, "Invalid player\n");
        return index;
    }
    entity character = get_linked_character(world, player);
    if (!zox_valid(character)) {
        index += snprintf(buffer + index, size - index, "[%s] has no character\n", zox_get_name(player));
        return index;
    }
    zox_geter(character, RaycastVoxelData, data);
    index += snprintf(buffer + index, size - index, "Light Debugger\n");
    index += snprintf(buffer + index, size - index, " - Raycast Result [%i]\n", data->result);
    // node debugger
    if (!data->node) {
        index += snprintf(buffer + index, size - index, " - No Voxel Node\n");
        return index;
    }
    entity chunk_last = data->chunk_last;
    if (zox_valid(chunk_last)) {
        zox_geter_value(chunk_last, RenderDepth, byte, depth_last);
        zox_geter(chunk_last, LightNode, light_node_last);
        byte light_last = get_value_LightNode(light_node_last, depth_last, data->positionl_last, 0);
        index += snprintf(buffer + index, size - index, "Last Chunk [%s]\n", zox_get_name(chunk_last));
        index += snprintf(buffer + index, size - index, " - Depth [%i]\n", depth_last);
        index += snprintf(buffer + index, size - index, " - Position [%ix%ix%i]\n", data->positionl_last.x, data->positionl_last.y, data->positionl_last.z);
        index += snprintf(buffer + index, size - index, " - Light Air [%i]\n", light_last);
        /*if (depth_last) {
            byte light_air_upper = get_value_LightNode(light_node_last, depth_last - 1, data->positionl_last, 0);
            index += snprintf(buffer + index, size - index, " - Light Air (+1 depth) [%i]\n", light_air_upper);
        }*/
    } else {
        index += snprintf(buffer + index, size - index, "Chunk Last Invalid\n");
        // return index;
    }
    // data->chunk now chunk_last
    // NOTE: This gets adjacent data from hit voxel!
    if (zox_valid(data->chunk) && zox_has(data->chunk, ChunkNeighbors)) {
        zox_geter(data->chunk, LightNode, hit_light_octree);
        zox_geter_value(data->chunk, RenderDepth, byte, depth_hit);
        byte face = normal_to_direction(data->normal);
        zox_geter(data->chunk, ChunkNeighbors, neighbors);
        const LightNode* neighbor_light_octrees[6];
        fetch_neightbor_light_nodes(world, neighbors, neighbor_light_octrees);
        if (!neighbor_light_octrees[0]) {
            entity n = neighbors->value[0];
            zox_loge("Null Neighbor, weirdness: %s", zox_valid(n) ? zox_get_name(n) : "invalid");
            return index;
        }
        // later we can get all lights nearby
        const LightNode* adjacent_light = get_neighbor_LightNode(hit_light_octree, neighbor_light_octrees, face, data->positionl, depth_hit);
        byte adjacent_light_value = adjacent_light ? adjacent_light->value : 0;
        const LightNode* adjacent_max_light = get_max_light_on_face_start(hit_light_octree, neighbor_light_octrees, data->positionl, depth_hit, face);
        byte max_light = adjacent_max_light ? adjacent_max_light->value : 0;
        // Now print
        index += snprintf(buffer + index, size - index, "Hit Chunk [%s]\n", zox_get_name(data->chunk));
        index += snprintf(buffer + index, size - index, " - Depth [%i]\n", depth_hit);
        index += snprintf(buffer + index, size - index, " - Position [%ix%ix%i]\n", data->positionl.x, data->positionl.y, data->positionl.z);
        index += snprintf(buffer + index, size - index, " - Face [%i]\n", face);
        index += snprintf(buffer + index, size - index, " - Light [%i]\n", adjacent_light_value);
        index += snprintf(buffer + index, size - index, " - Max Light [%i]\n", max_light);
        if (adjacent_light) {
            // print children
            if (adjacent_light->ptr) {
                const LightNode* adjacent_light_children = (const LightNode*) adjacent_light->ptr;
                index += snprintf(buffer + index, size - index, " - Adjacent All\n");
                for (byte i = 0; i < 8; i++) {
                    const LightNode* kid = &adjacent_light_children[i];
                    index += snprintf(buffer + index, size - index, "  - Kid Light [%i] [%i]\n", i, kid->value);
                }
                index += snprintf(buffer + index, size - index, " - Adjacent Only\n");
                const byte* indexes = octree_face_children[face];
                for (byte i = 0; i < 4; i++) {
                    const LightNode* kid = &adjacent_light_children[indexes[i]];
                    index += snprintf(buffer + index, size - index, "  - Kid Light [%i] [%i]\n", indexes[i], kid->value);
                }
            }
        }
    }
    /*entity chunk_hit = data->chunk;
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
        data->positionv_last.x, data->positionv_last.y, data->positionv_last.z);*/

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
    }
    index += snprintf(buffer + index, size - index, "]\n");
    byte light = get_value_LightNode(light_node_hit, depth_hit, data->positionl, 0);
    index += snprintf(buffer + index, size - index, "   + inside light [%i]\n", light);*/
    return index;
}