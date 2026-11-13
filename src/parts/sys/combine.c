ushort max_model_length = 256;

// Returns the vox of a part model, voxels are always at max depth
entity item_get_max_depth_vox(ecs* world, entity part) {
    if (!zox_valid(part)) {
        return 0;
    }
    if (!zox_is_linked_any(world, part, ModelLink)) {
        zox_loge("Part has no Model Link [%s]", zox_get_name(part));
        return 0;
    }
    entity vox = get_model_first(world, zox_get_link(world, part, ModelLink));
    if (!zox_valid(vox)) {
        zox_loge("[player body]: Invalid Part %s Model", zox_get_name(part));
        return 0;
    }
    if (!zox_has(vox, VoxelNode) || !zox_has(vox, NodeDepth)) {
        zox_loge("Part Vox Invalid Components %s Model %s", zox_get_name(part), zox_get_name(vox));
        return 0;
    }
    return vox;
}

// NOTE: Recursively add parts to the body
// TODO: Get position based on previous slot
void build_body_parts(ecs *world, entity slot, CombineList* voxes, CombinePositions* positions, CombineDepths* depths, entity_array_d* slots_used, byte3* body_size, byte* max_depth, byte dbg_log) {
    entity part = zox_get_link(world, slot, DataLink);
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
    // NOTE: Parts are sampled at the parts depth, models only hold max depth voxels
    byte part_depth = get_item_model_depth(world, part, vox);
    int3 part_size = get_item_model_size(world, part, vox);
    if (part_max_depth > *max_depth) {
        *max_depth = part_max_depth;
        if (dbg_log) {
            short length = octree_size(*max_depth);
            zox_log("Setting Body Depth [%i] v[%i]", *max_depth, length);
        }
    }
    // Get Parent Data
    entity parent_slot = zox_get_parent(world, slot);
    byte3 parent_position = parent_slot > 0 && zox_has(parent_slot, PartPosition) ? zox_getv(parent_slot, PartPosition) : byte3_zero;
    int3 parent_size = parent_slot > 0 && zox_has(parent_slot, PartSize) ? byte3_to_int3(zox_getv(parent_slot, PartSize)) : int3_zero;
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
        part_position.x += (parent_size.x - part_size.x) / 2;
        part_position.y = parent_position.y + offset.y - part_size.y;
        part_position.z += (parent_size.z - part_size.z) / 2;
    } else if (anchor == body_anchor_left) {
        part_position.x = parent_position.x + offset.x - part_size.x;
        part_position.y += (parent_size.y - part_size.y) / 2;
        part_position.z += (parent_size.z - part_size.z) / 2;
    } else if (anchor == body_anchor_back) {
        part_position.x += (parent_size.x - part_size.x) / 2;
        part_position.z = parent_position.z + offset.z - part_size.z;
        part_position.y += (parent_size.y - part_size.y) / 2;
    } else if (anchor == body_anchor_over) {
        // clothes: centred over the parent on x and z, from its bottom up, a bigger part sticks out
        //  (under 0 shifts the grid below, past the body's size grows it here)
        part_position.x += (parent_size.x - part_size.x) / 2;
        part_position.z += (parent_size.z - part_size.z) / 2;
        const int3 upper = int3_add(part_position, part_size);
        body_size->x = upper.x > body_size->x ? upper.x : body_size->x;
        body_size->y = upper.y > body_size->y ? upper.y : body_size->y;
        body_size->z = upper.z > body_size->z ? upper.z : body_size->z;
    }
    short vlength = octree_size(*max_depth);
    if (body_size->x >= vlength || body_size->y >= vlength || body_size->z >= vlength) {
        *max_depth = *max_depth + 1;
        short new_vlength = octree_size(*max_depth);
        if (dbg_log) {
            zox_log("Expanding Body Depth [%i] v[%i]", *max_depth, new_vlength);
            zox_log(" - Body Size [%ix%ix%i] > Grid Size [%i]", body_size->x, body_size->y, body_size->z, vlength);
        }
    }
    // Shift Grid using Part Position per dimension
    if (part_position.y < 0) {
        int boost = -part_position.y;
        part_position.y = 0;
        // Increase Body Grid Size
        int new_dimension = body_size->y + boost;
        if (new_dimension >= max_model_length) {
            zox_log("[Y] Body Past Bounds [%i] > 128", new_dimension);
            return;
        }
        body_size->y = new_dimension;
        // Move all previous positions up
        for (uint k = 0; k < positions->length; k++) {
            byte3 position = positions->value[k];
            entity slot_used = slots_used->data[k];
            zox_muter(slot_used, PartPosition, slot_position);
            position.y += boost;
            slot_position->value.y += boost;
            positions->value[k] = position;
        }
    }
    if (part_position.x < 0) {
        int boost = -part_position.x;
        part_position.x = 0;
        // Increase Body Grid Size
        int new_dimension = body_size->x + boost;
        if (new_dimension >= max_model_length) {
            zox_logw("[X] Body Past Bounds [%i] > 128", new_dimension);
            return;
        }
        body_size->x = new_dimension;
        // Move all previous positions up
        for (uint k = 0; k < positions->length; k++) {
            byte3 position = positions->value[k];
            entity slot_used = slots_used->data[k];
            zox_muter(slot_used, PartPosition, slot_position);
            position.x += boost;
            slot_position->value.x += boost;
            positions->value[k] = position;
        }
    }
    if (part_position.z < 0) {
        int boost = -part_position.z;
        part_position.z = 0;
        // Increase Body Grid Size
        int new_dimension = body_size->z + boost;
        if (new_dimension >= max_model_length) {
            zox_logw("[Z] Body Past Bounds [%i] > 128", new_dimension);
            return;
        }
        body_size->z = new_dimension;
        // Move all previous positions up
        for (uint k = 0; k < positions->length; k++) {
            byte3 position = positions->value[k];
            entity slot_used = slots_used->data[k];
            zox_muter(slot_used, PartPosition, slot_position);
            position.z += boost;
            slot_position->value.z += boost;
            positions->value[k] = position;
        }
    }
    // Set position and size here
    byte3 part_position_b3 = int3_to_byte3(part_position);
    byte3 part_size_b3 = int3_to_byte3(part_size);
    zox_muter(slot, PartPosition, part_position_com);
    zox_muter(slot, PartSize, part_size_com);
    part_position_com->value = part_position_b3;
    part_size_com->value = part_size_b3;
    // Store them
    add_to_CombineList(voxes, vox);
    add_to_CombinePositions(positions, part_position_b3);
    add_to_CombineDepths(depths, part_depth);
    entity_array_d_add(slots_used, slot);
    if (dbg_log) {
        zox_log("Combining Part [%s] a[%i]", zox_get_name(vox), anchor);
        zox_log("   Place Position [%ix%ix%i]", part_position_b3.x, part_position_b3.y, part_position_b3.z);
        zox_log("   Part Size [%ix%ix%i]", part_size.x, part_size.y, part_size.z);
        zox_log("   Body Size [%ix%ix%i]", body_size->x, body_size->y, body_size->z);
        zox_log("   Parent Position [%ix%ix%i]", parent_position.x, parent_position.y, parent_position.z);
        zox_log("   Parent Size [%ix%ix%i]", parent_size.x, parent_size.y, parent_size.z);
    }
    // Recursive add
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, slot, slots, zox_children_capacity, zox_id(Slot));
    if (dbg_log) {
        zox_log("[%s] has [%i] Children in Body Building", zox_get_name(slot), length);
    }
    // clothes (anchor over) last: later parts are drawn over earlier ones, so a shirt's sleeves cover the shoulders
    for (byte clothes = 0; clothes < 2; clothes++) {
        for (uint k = 0; k < length; k++) {
            entity child_slot = slots[k];
            if ((zox_getv(child_slot, SlotAnchor) == body_anchor_over) != clothes) {
                continue;
            }
            if (dbg_log) {
                zox_log("   - [%i]:[%s]", k, zox_get_name(child_slot));
            }
            build_body_parts(world, child_slot, voxes, positions, depths, slots_used, body_size, max_depth, dbg_log);
        }
    }
}

// TODO: The placement should know the Position + Size of the vox we are attaching to
// for now just set vox, later we spawn item and set it from BodyDirty
// TODO: When Combining a part, remember where the parent position is, add them in recursively instead of the flat way atm
// NOTE: Combines into the characters child vox (VoxLink), each child vox belongs to one character
void body_combine_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(BodyDirty);
    zox_sys_out(BodySize);
    zox_sys_out(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(BodyDirty, body_dirty);
        zox_sys_o(BodySize, body_size);
        zox_sys_o(BlockScale, block_scale);
        if (body_dirty->value != zox_generate_body_combine) {
            continue;
        }
        // NOTE: Waits for the vox to spawn
        entity vox = zox_get_link(world, e, VoxLink);
        if (!zox_valid(vox) || !zox_has(vox, CombineList)) {
            continue;
        }
        entity chest_slot = zox_get_child_by_id(world, e, zox_id(Body));
        if (!zox_valid(chest_slot)) {
            continue;
        }
        CombineList* voxes = zox_getm(vox, CombineList);
        CombinePositions* positions = zox_getm(vox, CombinePositions);
        CombineDepths* depths = zox_getm(vox, CombineDepths);
        byte max_depth = 0;
        body_size->value = byte3_zero;
        resize_CombineList(voxes, 0);
        resize_CombinePositions(positions, 0);
        resize_CombineDepths(depths, 0);
        entity_array_d* slots_used = create_entity_array_d(1);
        build_body_parts(world, chest_slot, voxes, positions, depths, slots_used, &body_size->value, &max_depth, dbg_log);
        dispose_entity_array_d(slots_used);
        // NOTE: Keep at consistent scale
        // TODO: We should make this same scale as npcs
        short length2 = octree_size(block_depth + 2);
        block_scale->value = 1.0f / length2;
        body_dirty->value = zox_generate_body_bones;
        zox_setm(vox, NodeDepth, max_depth);
        zox_setm(vox, BlockScale, block_scale->value);
        zox_setm(vox, CombineVox, zox_dirty_trigger);
        // NOTE: Render at the depth for our chunks distance, like chunk_entities_lod_system
        zox_setm(e, MaxRenderDepth, max_depth);
        byte distance = 0;
        if (zox_is_linked_any(world, e, ChunkLink)) {
            entity chunk = zox_get_link(world, e, ChunkLink);
            if (zox_valid(chunk) && zox_has(chunk, RenderDistance)) {
                distance = zox_getv(chunk, RenderDistance);
            }
        }
        byte render_depth = camera_distance_to_npc_render_depth(distance, max_depth);
        if (zox_getv(e, RenderDepth) != render_depth) {
            zox_setm(e, RenderDepth, render_depth);
            zox_setm(e, RenderDepthDirty, zox_dirty_trigger);
        }
        if (dbg_log) {
            zox_log("Body [%s] Depth [%i] Scale [%f]", zox_get_name(e), max_depth, block_scale->value);
        }
    }
    zox_sys_on_end();
} zoxd_system(body_combine_system);
