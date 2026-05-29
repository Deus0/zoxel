byte max_model_length = 128;

entity item_get_max_depth_vox(ecs* world, entity part) {
    if (!zox_valid(part)) {
        return 0;
    }
    if (!zox_has(part, ModelLink)) {
        zox_log_error("Part has no Model Link [%s]", zox_get_name(part));
        return 0;
    }
    entity vox = zox_getv(part, ModelLink);
    if (!zox_valid(vox)) {
        zox_log_error("[player body]: Invalid Part %s Model", zox_get_name(part));
        return 0;
    }
    if (!zox_has(vox, MaxRenderDepth)) {
        zox_log_error("Part Vox Invalid Components %s Model %s", zox_get_name(part), zox_get_name(vox));
        return 0;
    }
    byte max_render_depth = zox_getv(part, MaxRenderDepth);
    if (zox_has(vox, ModelLods)) {
        zox_geter(vox, ModelLods, mlods);
        entity vox_lod = mlods->value[max_render_depth];
        if (!zox_valid(vox_lod)) {
            zox_logw("Part [%s] has invalid Vox Lod [%i]", zox_get_name(part), max_render_depth);
            return 0;
        }
        return vox_lod;
    }
    return vox;
}

// NOTE: Recursively add parts to the body
// TODO: Get position based on previous slot
void build_body_parts(ecs *world, entity slot, CombineList* voxes, CombinePositions* positions, entity_array_d* slots_used, byte3* body_size, byte* max_depth, byte dbg_log) {
    entity part = zox_getv(slot, DataLink);
    byte anchor = zox_getv(slot, SlotAnchor);
    if (!zox_valid(part)) {
        return;
    }
    if (!zox_has(part, MaxRenderDepth)) {
        zox_loge("Part [%s] has no MaxRenderDepth", zox_get_name(part));
        return;
    }
    byte part_max_depth = zox_getv(part, MaxRenderDepth);
    // Get Vox
    entity vox = item_get_max_depth_vox(world, part);
    if (!zox_valid(vox)) {
        zox_loge("Part [%s] has Invalid Vox", zox_get_name(part));
        return;
    }
    if (!zox_has(vox, ChunkSize)) {
        zox_loge("Part [%s]'s Vox [%s] has no ChunkSize", zox_get_name(part), zox_get_name(vox));
        return;
    }
    byte3 part_size = int3_to_byte3(zox_getv(vox, ChunkSize));
    if (part_max_depth > *max_depth) {
        *max_depth = part_max_depth;
        if (dbg_log) {
            byte new_vlength = powers_of_two_byte[*max_depth];
            zox_log("Setting Body Depth [%i] v[%i]", *max_depth, new_vlength);
        }
    }
    // Get Parent Data
    entity parent_slot = zox_get_parent(world, slot);
    byte3 parent_position = parent_slot > 0 && zox_has(parent_slot, PartPosition) ? zox_getv(parent_slot, PartPosition) : byte3_zero;
    byte3 parent_size = parent_slot > 0 && zox_has(parent_slot, PartSize) ? zox_getv(parent_slot, PartSize) : byte3_zero;
    // Calculate Placement Position
    // set part position to parent
    int3 offset = zox_has(slot, PartOffset) ? zox_getv(slot, PartOffset) : int3_zero;
    int3 part_position = int3_add(byte3_to_int3(parent_position), offset);
    // Anchor position from parent
    if (anchor == body_anchor_core) {
        body_size->x = part_size.x;
        body_size->y = part_size.y;
        body_size->z = part_size.z;
    } else if (anchor == body_anchor_top) {
        // Check the placement upper bounds
        part_position.y += parent_size.y;
        int part_upper_y = part_position.y + part_size.y;
        if (part_upper_y >= max_model_length) {
            zox_loge("Part [%s] OOB: Y [%i of %i]", zox_get_name(vox), part_upper_y, max_model_length);
            zox_loge(" Parent At [%ix%ix%i]", parent_position.x, parent_position.y, parent_position.z);
            zox_loge(" Parent Size [%ix%ix%i]", parent_size.x, parent_size.y, parent_size.z);
            zox_loge(" Offset [%ix%ix%i]", offset.x, offset.y, offset.z);
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
        part_position.x += parent_size.x;
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
        // Check the placement upper bounds
        part_position.z += parent_size.z;
        int part_upper = part_position.z + part_size.z;
        if (part_upper >= max_model_length) {
            zox_log("Size outside bounds for part X [%i]", part_upper);
            return;
        }
        // Increase Grid Size if needed
        if (part_upper > body_size->z) {
            body_size->z = part_upper;
        }
        // PositionXY
        part_position.x += (parent_size.x - part_size.x) / 2;
        part_position.y += (parent_size.y - part_size.y) / 2;
    } else if (anchor == body_anchor_bottom) {
        // Calculate the Bottom slot Grid Difference
        int part_position_y = parent_position.y + offset.y - part_size.y;
        if (part_position_y >= 0) {
            part_position.y = part_position_y;
        } else {
            part_position.y = 0;
            // Increase Body Grid Size
            int boost_y = -part_position_y; // Distance to Ground
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
        // Position XZ
        part_position.x += (parent_size.x - part_size.x) / 2;
        part_position.z += (parent_size.z - part_size.z) / 2;
    } else if (anchor == body_anchor_left) {
        // Calculate the Left slot Grid Difference
        int part_position_x = parent_position.x - part_size.x + offset.x;
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
        // Position YZ
        part_position.y += (parent_size.y - part_size.y) / 2;
        part_position.z += (parent_size.z - part_size.z) / 2;
    } else if (anchor == body_anchor_back) {
        // Calculate the Left slot Grid Difference
        int part_position_z = parent_position.z - part_size.z + offset.z;
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
        // Position XY
        part_position.x += (parent_size.x - part_size.x) / 2;
        part_position.y += (parent_size.y - part_size.y) / 2;
    }
    // uint lower_bounds_y = part_position.y;
    byte vlength = powers_of_two_byte[*max_depth];
    if (body_size->x >= vlength || body_size->y >= vlength || body_size->z >= vlength) {
        *max_depth = *max_depth + 1;
        byte new_vlength = powers_of_two_byte[*max_depth];
        if (dbg_log) {
            zox_log("Expanding Body Depth [%i] v[%i]", *max_depth, new_vlength);
            zox_log(" - Body Size [%ix%ix%i] > Grid Size [%i]", body_size->x, body_size->y, body_size->z, vlength);
        }
    }
    // Set position and size here
    byte3 part_position_b3 = int3_to_byte3(part_position);
    zox_muter(slot, PartPosition, part_position_com);
    zox_muter(slot, PartSize, part_size_com);
    part_position_com->value = part_position_b3;
    part_size_com->value = part_size;
    // Store them
    add_to_CombineList(voxes, vox);
    add_to_CombinePositions(positions, part_position_b3);
    add_to_entity_array_d(slots_used, slot);
    if (dbg_log) {
        zox_log("Combining Part [%s] a[%i]", zox_get_name(vox), anchor);
        zox_log("   pos [%ix%ix%i]", part_position_b3.x, part_position_b3.y, part_position_b3.z);
        zox_log("   size [%ix%ix%i]", part_size.x, part_size.y, part_size.z);
        zox_log("   body [%ix%ix%i]", body_size->x, body_size->y, body_size->z);
    }
    // Recursive add
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, slot, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        entity child_slot = slots[k];
        build_body_parts(world, child_slot, voxes, positions, slots_used, body_size, max_depth, dbg_log);
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
        body_size->value = byte3_zero;
        resize_CombineList(voxes, 0);
        resize_CombinePositions(positions, 0);
        entity_array_d* slots_used = create_entity_array_d(1);
        build_body_parts(world, chest_slot, voxes, positions, slots_used, &body_size->value, &max_depth, dbg_log);
        dispose_entity_array_d(slots_used);
        // This should be calculated when we add to our vox?
        // TODO: We should make this same scale as npcs
        ndepth->value = max_depth;
        // NOTE: Keep at consistent scale
        block_scale->value = 1.0f / powers_of_two_byte[block_vox_depth + 2];
        dirty->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("Body [%s] Depth [%i] Scale [%f]", zox_get_name(e), max_depth, block_scale->value);
        }
    }
} zox_sys_end(BodyCombineSystem);
