void combine_vox_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    ushort max_colors = 255;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CombineVox);
    zox_sys_in(CombineList);
    zox_sys_in(CombinePositions);
    zox_sys_out(ChunkSize);
    zox_sys_out(NodeDepth);
    zox_sys_out(RenderDepth);
    zox_sys_out(VoxelNode);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CombineVox, state);
        zox_sys_i(CombineList, voxes);
        zox_sys_i(CombinePositions, positions);
        zox_sys_o(ChunkSize, csize);
        zox_sys_o(NodeDepth, ndepth);
        zox_sys_o(RenderDepth, rdepth);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(ColorRGBs, colors);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (voxes->length != positions->length) {
            zox_loge("Vox and Positions out of Sync, cannot combine.");
            continue;
        }
        // zox_log("Combining Voxes [%i]", voxes->length);
        // NOTE: Optional depths to sample each vox at, as models only store max depth voxels
        const CombineDepths* depths = zox_has(e, CombineDepths) ?
            zox_get(e, CombineDepths) : NULL;
        if (depths && depths->length != voxes->length) {
            depths = NULL;
        }
        // TODO: calculate the overall size first
        // add each vox to the current node
        int3 new_csize = int3_zero;
        // set voctree here
        resize_ColorRGBs(colors, 0);
        // TODO: set voctree depth and set to air
        // its octree locked while written, readers can run alongside (thread_safety.md)
        spinlock* write_lock = zox_octree_lock(world, e);
        zox_octree_lock_begin(write_lock);
        // Clear octree
        voctree->value = 0;
        collapse_VoxelNode(voctree);
        for (byte j = 0; j < voxes->length; j++) {
            entity vox = voxes->value[j];
            if (!zox_valid(vox)) {
                continue;
            }
            if (!zox_has(vox, NodeDepth)) {
                zox_loge("Combining Vox [%s] has no [NodeDepth]", zox_get_name(vox));
                continue;
            }
            if (!zox_has(vox, ChunkSize)) {
                zox_loge("Combining Vox [%s] has no [ChunkSize]", zox_get_name(vox));
                continue;
            }
            /*if (ndepth->value != zox_getv(vox, NodeDepth)) {
                zox_logw("We don't support adding different depths yet");
                continue;
            }*/
            // NOTE: Colors, we will need to make a lookup table for index placement
            byte3 vposition = positions->value[j];
            byte vox_depth = zox_getv(vox, NodeDepth);
            int3 vox_size = zox_getv(vox, ChunkSize);
            if (depths && depths->value[j] < vox_depth) {
                vox_size = get_vox_size_at_depth(vox_size, vox_depth, depths->value[j]);
                vox_depth = depths->value[j];
            }
            const VoxelNode* vox_octree = zox_get(vox, VoxelNode);
            const ColorRGBs* vox_colors = zox_get(vox, ColorRGBs);
            // get max octree type
            byte max_color_index = get_max_value_VoxelNode(vox_octree);
            if (dbg_log) {
                zox_log("Vox [%i] max_color_index for vox [%s] [%i]", j, zox_get_name(vox), max_color_index);
            }
            for (ushort k = 0; k < max_color_index && colors->length < max_colors; k++) {
                // NOTE: Added check to make sure color is actually inside vox, for imported voxes
                byte is_in = is_in_VoxelNode(vox_octree, k + 1);
                if (!is_in) {
                    continue;
                }
                color_rgb acolor = vox_colors->value[k];
                // If not in list
                byte already_added = 0;
                for (ushort l = 0; l < colors->length; l++) {
                    color_rgb base_color = colors->value[l];
                    if (color_rgb_equals(base_color, acolor)) {
                        already_added = 1;
                        break;
                    }
                }
                if (!already_added) {
                    add_to_ColorRGBs(colors, acolor);
                }
            }
            // NOTE: For now lets assume same depth
            // add aoctree into voctree
            // account for depth difference
            // position, size for placement into new grid?
            // combine colors too
            ushort3 max_placement = (ushort3) {
                vposition.x + vox_size.x,
                vposition.y + vox_size.y,
                vposition.z + vox_size.z
            };
            ushort3 lposition = ushort3_zero;
            ushort3 position;
            for (lposition.x = 0, position.x = vposition.x; position.x < max_placement.x; position.x++, lposition.x++) {
                for (lposition.y = 0, position.y = vposition.y; position.y < max_placement.y; position.y++, lposition.y++) {
                    for (lposition.z = 0, position.z = vposition.z; position.z < max_placement.z; position.z++, lposition.z++) {
                        byte place_vox_value = getv_VoxelNode(vox_octree, vox_depth, ushort3_to_byte3(lposition));
                        if (!place_vox_value) {
                            continue;
                        }
                        // Convert to our placement
                        // get color from value
                        color_rgb place_vox_color = vox_colors->value[place_vox_value - 1];
                        // find color in place vox
                        byte value = 0;
                        // TODO: Use Colors Dictionary!
                        for (ushort k = 0; k < colors->length; k++) {
                            color_rgb body_color = colors->value[k];
                            if (color_rgb_equals(body_color, place_vox_color)) {
                                value = k + 1;  // + 1 for air
                                break;
                            }
                        }
                        // didn't find, maybe over max?
                        if (!value) {
                            continue;
                        }
                        byte3 positionb = ushort3_to_byte3(position);
                        set_VoxelNode(voctree, ndepth->value, positionb, value);
                        // Expands the size of our vox
                        if (position.x >= new_csize.x) {
                            new_csize.x = position.x;
                        }
                        if (position.y >= new_csize.y) {
                            new_csize.y = position.y;
                        }
                        if (position.z >= new_csize.z) {
                            new_csize.z = position.z;
                        }
                        // zox_log("At [%ix%ix%i] placed [%i] - from [%ix%ix%i]", position.x, position.y, position.z, value, gposition.x, gposition.y, gposition.z);
                    }
                }
            }
            // zox_log(" + vox %i [%s] c[%i] at [%ix%ix%i] of s[%ix%ix%i]", j, zox_get_name(vox), acolors->length, vposition.x, vposition.y, vposition.z, vox_size.x, vox_size.y, vox_size.z);
        }
        zox_octree_lock_end(write_lock);
        csize->value = new_csize;
        // NOTE: Vox data is always at max depth, its owners RenderDepth picks the mesh
        rdepth->value = ndepth->value;
        zox_add(e, VoxelNodeDirty);
        /*zox_log(" - colors [%i]", colors->length);
        zox_log(" - vdepth [%i] - grid max [%i]", ndepth->value, powers_of_two_byte[ndepth->value]);
        zox_log(" - csize [%ix%ix%i]", csize->value.x, csize->value.y, csize->value.z);
        zox_log(" - bscale [%f]", bscale->value);*/
    }
    zox_sys_on_end();
} zoxd_system(combine_vox_system);

        /*byte new_depth = 0;
        // float new_scale = 0;
        for (byte j = 0; j < voxes->length; j++) {
            entity vox = voxes->value[j];
            if (!zox_valid(vox)) {
                continue;
            }
            byte vdepth = zox_getv(vox, NodeDepth);
            if (vdepth > new_depth) {
                new_depth = vdepth;
            }
            if (vdepth == new_depth) {
                int3 vox_size = zox_getv(vox, ChunkSize);
                if (vox_size.x > new_csize.x) new_csize.x = vox_size.x;
                if (vox_size.y > new_csize.y) new_csize.y = vox_size.y;
                if (vox_size.z > new_csize.z) new_csize.z = vox_size.z;
            }
        }
        ndepth->value = new_depth;*/
