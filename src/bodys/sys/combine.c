byte max_model_length = 128;

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
void build_body_r(ecs *world, entity slot, CombineList* voxes, CombinePositions* positions, entity_array_d* slots_used, byte3* body_size, byte* max_depth) {
    entity part = zox_getv(slot, DataLink);
    byte anchor = zox_getv(slot, SlotAnchor);
    if (!zox_valid(part)) {
        return;
    }
    entity vox = item_get_max_depth_vox(world, part);
    byte3 part_size = int3_to_byte3(zox_getv(vox, ChunkSize));
    byte part_max_depth = zox_getv(part, MaxRenderDepth);
    if (part_max_depth > *max_depth) {
        *max_depth = part_max_depth;
        zox_log("Setting Body Depth [%i]", *max_depth);
    }
    // Get Parent Data
    entity parent_slot = zox_get_parent(world, slot);
    // entity parent_part = (parent_slot > 0 && zox_has(parent_slot, DataLink)) ? zox_getv(parent_slot, DataLink) : 0;
    // entity parent_vox = item_get_max_depth_vox(world, parent_part);
    // int3 parent_size = parent_vox > 0 ? zox_getv(parent_vox, ChunkSize) : int3_zero;
    byte3 parent_position = parent_slot > 0 && zox_has(parent_slot, PartPosition) ? zox_getv(parent_slot, PartPosition) : byte3_zero;
    byte3 parent_size = parent_slot > 0 && zox_has(parent_slot, PartSize) ? zox_getv(parent_slot, PartSize) : byte3_zero;
    // Calculate Placement Position
    byte3 part_position = byte3_zero;
    // set part position to parent
    part_position = parent_position;
    // part_position = byte3_add(part_position, byte3_half(parent_size));
    if (anchor == body_anchor_core) {
        body_size->x = part_size.x;
        body_size->y = part_size.y;
        body_size->z = part_size.z;
    } else if (anchor == body_anchor_top) {
        // Check the placement upper bounds
        part_position.y = parent_position.y + parent_size.y;
        int part_upper_y = part_position.y + part_size.y;
        if (part_upper_y >= max_model_length) {
            zox_log("Size outside bounds for part Y [%i]", part_upper_y);
            return;
        }
        // Increase Grid Size if needed
        if (part_upper_y > body_size->y) {
            body_size->y = part_upper_y;
        }
        // Position XZ
        part_position.x += (parent_size.x - part_size.x) / 2;
        part_position.z += (parent_size.z - part_size.z) / 2;
    } else if (anchor == body_anchor_right) {
        // Check the placement upper bounds
        part_position.x = parent_position.x + parent_size.x;
        int part_upper_x = part_position.x + part_size.x;
        if (part_upper_x >= max_model_length) {
            zox_log("Size outside bounds for part X [%i]", part_upper_x);
            return;
        }
        // Increase Grid Size if needed
        if (part_upper_x > body_size->x) {
            body_size->x = part_upper_x;
        }
        // Position YZ
        part_position.y += (parent_size.y - part_size.y) / 2;
        part_position.z += (parent_size.z - part_size.z) / 2;
    } else if (anchor == body_anchor_forward) {
        // Position YZ
        part_position.y += (parent_size.y - part_size.y) / 2;
        part_position.x += (parent_size.x - part_size.x) / 2;
        // Check the placement upper bounds
        part_position.z = parent_position.z + parent_size.z;
        int part_upper = part_position.z + part_size.z;
        if (part_upper >= max_model_length) {
            zox_log("Size outside bounds for part X [%i]", part_upper);
            return;
        }
        // Increase Grid Size if needed
        if (part_upper > body_size->z) {
            body_size->z = part_upper;
        }
    } else if (anchor == body_anchor_bottom) {
        // Position XZ
        part_position.x += (parent_size.x - part_size.x) / 2;
        part_position.z += (parent_size.z - part_size.z) / 2;
        // Calculate the Bottom slot Grid Difference
        int part_position_y = parent_position.y - part_size.y;
        if (part_position_y >= 0) {
            part_position.y = part_position_y;
        } else {
            part_position.y = 0;
            // Increase Body Grid Size
            int boost_y = -part_position_y;
            int new_body_size_y = body_size->y + boost_y;
            if (new_body_size_y >= max_model_length) {
                zox_log("Body Needs resizing Past Bounds Y [%i]", new_body_size_y);
                return;
            }
            body_size->y = new_body_size_y;
            // Move all previous positions up
            for (uint k = 0; k < positions->length; k++) {
                byte3 position = positions->value[k];
                entity slot_used = slots_used->data[k];
                zox_muter(slot_used, PartPosition, slot_used_position);
                position.y += boost_y;
                slot_used_position->value.y += boost_y;
                positions->value[k] = position;
            }
        }
    } else if (anchor == body_anchor_left) {
        // Position YZ
        part_position.y += (parent_size.y - part_size.y) / 2;
        part_position.z += (parent_size.z - part_size.z) / 2;
        // Calculate the Left slot Grid Difference
        int part_position_x = parent_position.x - part_size.x;
        if (part_position_x >= 0) {
            part_position.x = part_position_x;
        } else {
            part_position.x = 0;
            // Increase Body Grid Size
            int boost_x = -part_position_x;
            int new_body_size_x = body_size->x + boost_x;
            if (new_body_size_x >= max_model_length) {
                zox_log("Body X Past Bounds Y [%i] > 128", new_body_size_x);
                return;
            }
            body_size->x = new_body_size_x;
            // Move all previous positions up
            for (uint k = 0; k < positions->length; k++) {
                byte3 position = positions->value[k];
                entity slot_used = slots_used->data[k];
                zox_muter(slot_used, PartPosition, slot_used_position);
                position.x += boost_x;
                slot_used_position->value.x += boost_x;
                positions->value[k] = position;
            }
        }
    } else if (anchor == body_anchor_back) {
        // Position YZ
        part_position.y += (parent_size.y - part_size.y) / 2;
        part_position.x += (parent_size.x - part_size.x) / 2;
        // Calculate the Left slot Grid Difference
        int part_position_z = parent_position.z - part_size.z;
        if (part_position_z >= 0) {
            part_position.z = part_position_z;
        } else {
            part_position.z = 0;
            // Increase Body Grid Size
            int boost = -part_position_z;
            int new_body_size_z = body_size->z + boost;
            if (new_body_size_z >= max_model_length) {
                zox_log("Body Z Past Bounds Z [%i] > 128", new_body_size_z);
                return;
            }
            body_size->z = new_body_size_z;
            // Move all previous positions up
            for (uint k = 0; k < positions->length; k++) {
                byte3 position = positions->value[k];
                entity slot_used = slots_used->data[k];
                zox_muter(slot_used, PartPosition, slot_used_position);
                position.z += boost;
                slot_used_position->value.z += boost;
                positions->value[k] = position;
            }
        }
    }
    // uint lower_bounds_y = part_position.y;
    byte vlength = powers_of_two_byte[*max_depth];
    if (body_size->x >= vlength || body_size->y >= vlength || body_size->z >= vlength) {
        *max_depth = *max_depth + 1;
        byte new_vlength = powers_of_two_byte[*max_depth];
        zox_log("Expanding Body Depth [%i] v[%i]", *max_depth, new_vlength);
        zox_log(" - Body Size [%ix%ix%i] > Grid Size [%i]", body_size->x, body_size->y, body_size->z, vlength);
    }
    // Set position and size here
    zox_muter(slot, PartPosition, part_position_com);
    zox_muter(slot, PartSize, part_size_com);
    part_position_com->value = part_position;
    part_size_com->value = part_size;
    // store them
    add_to_entity_array_d(slots_used, slot);
    add_to_CombineList(voxes, vox);
    add_to_CombinePositions(positions, part_position);
    // Recursive add
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, slot, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        entity child_slot = slots[k];
        build_body_r(world, child_slot, voxes, positions, slots_used, body_size, max_depth);
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
        // slots used inside it
        entity_array_d* slots_used = create_entity_array_d(1);
        build_body_r(world, chest_slot, voxes, positions, slots_used, &body_size->value, &max_depth);
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

        /*byte vlength = powers_of_two_byte[max_depth];
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
        }*/


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
            byte3 part_position = byte3_zero;
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
                part_position.x += (bsize->value.x - psize.x) / 2;
                part_position.z += (bsize->value.z - psize.z) / 2;
                part_position.y += bsize->value.y;
                bsize->value.y += psize.y;
            } else if (anchor == body_anchor_bottom) {
                if (bsize->value.y + psize.y >= 255) {
                    zox_log("Size outside bounds for part [%i]", (bsize->value.y + psize.y));
                    continue;
                }
                // add to bottomy / midxz
                part_position.x += (bsize->value.x - psize.x) / 2;
                part_position.z += (bsize->value.z - psize.z) / 2;
                // Move all previous positions up
                bsize->value.y += psize.y;
                for (uint k = 0; k < positions->length; k++) {
                    byte3 position = positions->value[k];
                    position.y += psize.y;
                    positions->value[k] = position;
                }
            }
            if (dbg_log) {
                zox_log("Adding Vox [%s] to position (%i)[%ix%ix%i] of size [%ix%ix%i]", zox_get_name(vox), anchor, part_position.x, part_position.y, part_position.z, psize.x, psize.y, psize.x);
            }
            add_to_entity_array_d(slots_used, slot);
            add_to_CombineList(voxes, vox);
            add_to_CombinePositions(positions, part_position);
        }
        byte max_depth = 0;
        byte vlength = 0;
        // NOTE: This sets centre positions on body parts
        // NOTE: This sets Body Octree Depth for the placement
        */
