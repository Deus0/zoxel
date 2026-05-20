entity item_get_max_depth_vox(ecs* world, entity part) {
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
        zox_sys_o(BodySize, bsize);
        zox_sys_o(CombineList, voxes);
        zox_sys_o(CombinePositions, positions);
        zox_sys_o(CombineVox, output);
        zox_sys_o(NodeDepth, ndepth);
        zox_sys_o(BlockScale, bscale);
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity chest_slot = zox_get_child_by_id(world, e, zox_id(Body));
        if (!zox_valid(chest_slot)) {
            continue;
        }
        // entity core_part = zox_get_child_by_id(world, e, zox_id(BodyPart));
        entity_array_d* slots = create_entity_array_d(1);
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
            entity vox = item_get_max_depth_vox(world, part);
            int3 psize = zox_getv(vox, ChunkSize);
            // Get slot type, SlotAnchor
            byte anchor = zox_getv(slot, SlotAnchor);
            byte3 pposition = byte3_zero;
            if (anchor == 0) {
                bsize->value.x += psize.x;
                bsize->value.y += psize.y;
                bsize->value.z += psize.z;
            } else if (anchor == 1) {
                // add to topy / midxz
                pposition.x += (bsize->value.x - psize.x) / 2;
                pposition.z += (bsize->value.z - psize.z) / 2;
                pposition.y += bsize->value.y;
                bsize->value.y += psize.y;
            } else if (anchor == 2) {
                // add to bottomy / midxz
                pposition.x += (bsize->value.x - psize.x) / 2;
                pposition.z += (bsize->value.z - psize.z) / 2;
                // Move all previous positions up
                bsize->value.y += psize.y;
                for (uint k = 0; k < j; k++) {
                    byte3 position = positions->value[k];
                    position.y += psize.y;
                    positions->value[k] = position;
                }
            }
            if (dbg_log) {
                zox_log("Adding Vox [%s] to position (%i)[%ix%ix%i] of size [%ix%ix%i]", zox_get_name(vox), anchor,
                        pposition.x, pposition.y, pposition.z,
                        psize.x, psize.y, psize.x);
            }
            add_to_CombineList(voxes, vox);
            add_to_CombinePositions(positions, pposition);
        }
        byte max_depth = 0;
        byte vlength = 0;
        for (uint j = 0; j < voxes->length; j++) {
            entity slot = slots->data[j];
            entity part = zox_getv(slot, DataLink);
            if (!zox_valid(part)) {
                continue;
            }
            byte part_max_depth = zox_getv(part, MaxRenderDepth);
            if (part_max_depth > max_depth) {
                max_depth = part_max_depth;
                vlength = powers_of_two_byte[max_depth];
            }
            entity vox = voxes->value[j];
            byte3 pposition = positions->value[j];
            int3 psize = zox_getv(vox, ChunkSize);
            byte3 position = pposition;
            position.x += psize.x / 2;
            position.y += psize.y / 2;
            position.z += psize.z / 2;
            zox_set(part, PartPosition, { position });
            if (dbg_log) {
                zox_log("Part [%s] Vox [%s] Centre Position [%ix%ix%i]", zox_get_name(part), zox_get_name(vox), position.x, position.y, position.x);
            }
            // Check if position + size bounds is outside our body bounds
            uint lower_bounds_y = pposition.y;
            uint upper_bounds_y = pposition.y + psize.y;
            if (upper_bounds_y >= vlength) {
                zox_log("Part is outside of bounds > Y [%i] > [%i]", upper_bounds_y, vlength);
                max_depth++;
                vlength = powers_of_two_byte[max_depth];
            }
        }
        dispose_entity_array_d(slots);
        // This should be calculated when we add to our vox?
        ndepth->value = max_depth;
        // TODO: We should make this same scale as npcs
        bscale->value = 1.0f / powers_of_two_byte[block_vox_depth + 2];
        output->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("Body [%s] Depth [%i] Scale [%f]", zox_get_name(e), max_depth, bscale->value);
        }
    }
} zox_sys_end(BodyCombineSystem);
