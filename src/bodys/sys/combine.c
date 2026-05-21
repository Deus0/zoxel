entity item_get_max_depth_vox(ecs* world, entity part) {
    if (!zox_valid(part)) {
        return 0;
    }
    if (!zox_has(part, ModelLink)) {
        zox_log_error("Part has no Model Link [%s]", zox_get_name(part));
        return 0;
    }
    zox_geter_value_non_const(part, ModelLink, entity, vox);
    if (!zox_valid(vox)) {
        zox_log_error("[player body]: Invalid Part %s Model", zox_get_name(part));
        return 0;
    }
    if (!zox_has(vox, MaxRenderDepth)) {
        zox_log_error("Part Vox Invalid Components %s Model %s", zox_get_name(part), zox_get_name(vox));
        return 0;
    }
    zox_geter_value(vox, MaxRenderDepth, byte, max_render_depth);
    if (zox_valid(vox) && zox_has(vox, ModelLods)) {
        zox_geter(vox, ModelLods, mlods);
        entity vox_lod = mlods->value[max_render_depth];
        if (zox_valid(vox_lod)) {
            vox = vox_lod;
        } else {
            zox_log_error("[player body]: Invalid Vox Model Lod [%s] Depth [%i]", zox_get_name(vox), max_render_depth);
        }
    }
    if (!zox_valid(vox)) {
        zox_logw("Part [%s] has invalid Vox", zox_get_name(part));
    }
    return vox;
}

// NOTE: Recursively add parts to the body
// TODO: Get position based on previous slot
void build_body_r(ecs *world, entity e, CombineList* voxes, CombinePositions* positions, entity_array_d* slots_used, byte3* body_size, byte* max_depth) {
    entity part = zox_getv(e, DataLink);
    if (!zox_valid(part)) {
        return;
    }
    byte anchor = zox_getv(e, SlotAnchor);
    entity vox = item_get_max_depth_vox(world, part);
    int3 psize = zox_getv(vox, ChunkSize);
    byte part_max_depth = zox_getv(part, MaxRenderDepth);
    if (part_max_depth > *max_depth) {
        *max_depth = part_max_depth;
        zox_log("Setting Body Depth [%i]", *max_depth);
    }
    // Get Parent Data
    entity parent_slot = zox_get_parent(world, e);
    entity parent_part = zox_has(parent_slot, DataLink) ? zox_gett_value(parent_slot, DataLink) : 0;
    entity parent_vox = item_get_max_depth_vox(world, parent_part);
    int3 parent_size = parent_vox > 0 ? zox_getv(parent_vox, ChunkSize) : int3_zero;
    // Calculate Placement Position
    byte3 pposition = byte3_zero;
    if (anchor == body_anchor_core) {
        body_size->x = psize.x;
        body_size->y = psize.y;
        body_size->z = psize.z;
    } else if (anchor == body_anchor_top) {
        if (body_size->y + psize.y >= 128) {
            zox_log("Size outside bounds for part [%i]", (body_size->y + psize.y));
            return;
        }
        // add to topy / midxz
        pposition.y += body_size->y;
        pposition.x += (body_size->x - psize.x) / 2;
        pposition.z += (body_size->z - psize.z) / 2;
        body_size->y += psize.y;
    } else if (anchor == body_anchor_bottom) {
        if (body_size->y + psize.y >= 128) {
            zox_log("Size outside bounds for part [%i]", (body_size->y + psize.y));
            return;
        }
        // add to bottomy / midxz
        pposition.x += (body_size->x - psize.x) / 2;
        pposition.z += (body_size->z - psize.z) / 2;
        // Move all previous positions up
        body_size->y += psize.y;
        for (uint k = 0; k < positions->length; k++) {
            byte3 position = positions->value[k];
            position.y += psize.y;
            positions->value[k] = position;
        }
    } else if (anchor == body_anchor_right) {
        if (body_size->x + psize.x >= 128) {
            zox_log("Size outside bounds for part [%i]", (body_size->x + psize.x));
            return;
        }
        // add to topy / midxz
        pposition.y += (body_size->y - psize.y) / 2;
        pposition.z += (body_size->z - psize.z) / 2;
        pposition.x += body_size->x;
        body_size->x += psize.x;
    } else if (anchor == body_anchor_left) {
        if (body_size->x + psize.x >= 128) {
            zox_log("Size outside bounds for part [%i]", (body_size->x + psize.x));
            return;
        }
        // add to bottomy / midxz
        pposition.y += (body_size->y - psize.y) / 2;
        pposition.z += (body_size->z - psize.z) / 2;
        // Move all previous positions up
        body_size->x += psize.x;
        for (uint k = 0; k < positions->length; k++) {
            byte3 position = positions->value[k];
            position.x += psize.x;
            positions->value[k] = position;
        }
    }
    // uint lower_bounds_y = pposition.y;
    byte vlength = powers_of_two_byte[*max_depth];
    if (body_size->x >= vlength || body_size->y >= vlength || body_size->z >= vlength) {
        *max_depth = *max_depth + 1;
        byte new_vlength = powers_of_two_byte[*max_depth];
        zox_log("Expanding Body Depth [%i] v[%i]", *max_depth, new_vlength);
        zox_log(" - Body Size [%ix%ix%i] > Grid Size [%i]", body_size->x, body_size->y, body_size->z, vlength);
    }
    add_to_entity_array_d(slots_used, e);
    add_to_CombineList(voxes, vox);
    add_to_CombinePositions(positions, pposition);
    // Recursive add
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, e, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        entity slot = slots[k];
        build_body_r(world, slot, voxes, positions, slots_used, body_size, max_depth);
    }
}

// TODO: The placement should know the Position + Size of the vox we are attaching to
// for now just set vox, later we spawn item and set it from BodyDirty
// TODO: When Combining a part, remember where the parent position is, add them in recursively instead of the flat way atm
zox_sys2(BodyCombineSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BodyDirty);
    zox_sys_out(BodySize);
    zox_sys_out(CombineList);
    zox_sys_out(CombinePositions);
    zox_sys_out(CombineVox);
    zox_sys_out(NodeDepth);
    zox_sys_out(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(BodyDirty, state);
        zox_sys_o(BodySize, body_size);
        zox_sys_o(CombineList, voxes);
        zox_sys_o(CombinePositions, positions);
        zox_sys_o(CombineVox, dirty);
        zox_sys_o(NodeDepth, ndepth);
        zox_sys_o(BlockScale, block_scale);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity chest_slot = zox_get_child_by_id(world, e, zox_id(Body));
        if (!zox_valid(chest_slot)) {
            continue;
        }
        byte max_depth = 0;
        entity_array_d* slots_used = create_entity_array_d(1);
        build_body_r(world, chest_slot, voxes, positions, slots_used, &body_size->value, &max_depth);
        byte vlength = powers_of_two_byte[max_depth];
        // Since positions get shuffled during build, we need to set centre positions at end
        for (uint j = 0; j < voxes->length; j++) {
            entity slot = slots_used->data[j];
            entity vox = voxes->value[j];
            int3 psize = zox_getv(vox, ChunkSize);
            byte3 centre_position = positions->value[j];
            centre_position.x += psize.x / 2;
            centre_position.y += psize.y / 2;
            centre_position.z += psize.z / 2;
            zox_set(slot, PartPosition, { centre_position });
            if (dbg_log) {
                zox_log("Part [%s] Vox [%s] Centre Position [%ix%ix%i]", zox_get_name(vox), zox_get_name(vox), centre_position.x, centre_position.y, centre_position.x);
            }
        }
        dispose_entity_array_d(slots_used);
        // This should be calculated when we add to our vox?
        // TODO: We should make this same scale as npcs
        ndepth->value = max_depth;
        block_scale->value = 1.0f / powers_of_two_byte[block_vox_depth + 2];
        dirty->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("Body [%s] Depth [%i] Scale [%f]", zox_get_name(e), max_depth, block_scale->value);
        }
    }
} zox_sys_end(BodyCombineSystem);



        /*
        entity_array_d* slots_used = create_entity_array_d(1);
        fetch_slots_r(world, slots, chest_slot);
        // Clear combine data
        bsize->value = byte3_zero;
        resize_CombineList(voxes, 0);
        resize_CombinePositions(positions, 0);
        for (uint j = 0; j < slots->size; j++) {
            entity slot = slots->data[j];
            entity part = zox_getv(slot, DataLink);
            if (!zox_valid(part)) {
                continue;
            }
            byte anchor = zox_getv(slot, SlotAnchor);
            entity vox = item_get_max_depth_vox(world, part);
            int3 psize = zox_getv(vox, ChunkSize);
            // Get Parent Data
            entity parent_slot = zox_get_parent(world, slot);
            entity parent_part = zox_has(parent_slot, DataLink) ? zox_gett_value(parent_slot, DataLink) : 0;
            entity parent_vox = item_get_max_depth_vox(world, parent_part);
            int3 parent_size = parent_vox > 0 ? zox_getv(parent_vox, ChunkSize) : int3_zero;
            // Calculate Placement Position
            byte3 pposition = byte3_zero;
            if (anchor == body_anchor_core) {
                bsize->value.x = psize.x;
                bsize->value.y = psize.y;
                bsize->value.z = psize.z;
            } else if (anchor == body_anchor_top) {
                if (bsize->value.y + psize.y >= 255) {
                    zox_log("Size outside bounds for part [%i]", (bsize->value.y + psize.y));
                    continue;
                }
                // add to topy / midxz
                pposition.x += (bsize->value.x - psize.x) / 2;
                pposition.z += (bsize->value.z - psize.z) / 2;
                pposition.y += bsize->value.y;
                bsize->value.y += psize.y;
            } else if (anchor == body_anchor_bottom) {
                if (bsize->value.y + psize.y >= 255) {
                    zox_log("Size outside bounds for part [%i]", (bsize->value.y + psize.y));
                    continue;
                }
                // add to bottomy / midxz
                pposition.x += (bsize->value.x - psize.x) / 2;
                pposition.z += (bsize->value.z - psize.z) / 2;
                // Move all previous positions up
                bsize->value.y += psize.y;
                for (uint k = 0; k < positions->length; k++) {
                    byte3 position = positions->value[k];
                    position.y += psize.y;
                    positions->value[k] = position;
                }
            }
            if (dbg_log) {
                zox_log("Adding Vox [%s] to position (%i)[%ix%ix%i] of size [%ix%ix%i]", zox_get_name(vox), anchor, pposition.x, pposition.y, pposition.z, psize.x, psize.y, psize.x);
            }
            add_to_entity_array_d(slots_used, slot);
            add_to_CombineList(voxes, vox);
            add_to_CombinePositions(positions, pposition);
        }
        byte max_depth = 0;
        byte vlength = 0;
        // NOTE: This sets centre positions on body parts
        // NOTE: This sets Body Octree Depth for the placement
        */
