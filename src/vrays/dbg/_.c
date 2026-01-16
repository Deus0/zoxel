extern entity get_linked_character(ecs*, entity);

uint debug_ui_raycasting(ecs *world, entity player, char *buffer, uint size, uint index) {

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

    index += snprintf(buffer + index, size - index, "   + voxel_scale [%f]\n", data->voxel_scale);

    // index += snprintf(buffer + index, size - index, "   * sides [%i]\n", data->node->sides);


    return index;
}

#include "light.c"