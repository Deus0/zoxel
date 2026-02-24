zox_sys2(CombineVoxSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CombineVox);
    zox_sys_in(CombineList);
    zox_sys_in(CombinePositions);
    zox_sys_out(BlockScale);
    zox_sys_out(ChunkSize);
    zox_sys_out(NodeDepth);
    zox_sys_out(RenderDepth);
    zox_sys_out(VoxelNode);
    zox_sys_out(ColorRGBs);
    zox_sys_out(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CombineVox, state);
        zox_sys_i(CombineList, voxes);
        zox_sys_i(CombinePositions, positions);
        zox_sys_o(BlockScale, bscale);
        zox_sys_o(ChunkSize, csize);
        zox_sys_o(NodeDepth, ndepth);
        zox_sys_o(RenderDepth, rdepth);
        zox_sys_o(VoxelNode, voctree);
        zox_sys_o(ColorRGBs, colors);
        zox_sys_o(VoxelNodeDirty, dirty);

        if (state->value != zox_dirty_active) {
            continue;
        }

        if (voxes->length != positions->length) {
            zox_log_error("Vox and Positions out of Sync, cannot combine.");
            continue;
        }

        // zox_log("Combining Voxes [%i]", voxes->length);

        // TODO: calculate the overall size first

        // add each vox to the current node
        int3 new_csize = int3_zero;
        byte new_depth = 0;
        float new_scale = 0;
        for (byte j = 0; j < voxes->length; j++) {
            entity vox = voxes->value[j];

            if (!zox_valid(vox)) {
                continue;
            }

            zox_geter_value(vox, NodeDepth, byte, vdepth);

            if (vdepth > new_depth) {
                new_depth = vdepth;
            }

            if (vdepth == new_depth) {
                zox_geter_value(vox, ChunkSize, int3, vsize);

                if (vsize.x > new_csize.x) new_csize.x = vsize.x;
                if (vsize.y > new_csize.y) new_csize.y = vsize.y;
                if (vsize.z > new_csize.z) new_csize.z = vsize.z;

                zox_geter_value(vox, BlockScale, float, vscale);
                if (!bscale->value || vscale < bscale->value) bscale->value = vscale;
            }
        }

        ndepth->value = new_depth;
        rdepth->value = ndepth->value;

        // set voctree here
        resize_ColorRGBs(colors, 0);

        // TODO: set voctree depth and set to air

        write_lock_VoxelNode(voctree);

        for (byte j = 0; j < voxes->length; j++) {
            entity vox = voxes->value[j];

            // if (j == 1) continue;

            if (!zox_valid(vox)) {
                continue;
            }

            if (ndepth->value != zox_gett_value(vox, NodeDepth)) {
                zox_logw("We don't support adding different depths yet");
                continue;
            }

            // NOTE: Colors, we will need to make a lookup table for index placement

            byte3 vposition = positions->value[j];

            zox_geter(vox, VoxelNode, aoctree);
            zox_geter(vox, ColorRGBs, acolors);
            zox_geter_value(vox, ChunkSize, int3, vsize);

            // for now we just set to vox
            /*if (acolors->length > colors->length) {
                colors->length = acolors->length;
                int clength = sizeof(color_rgb) * acolors->length;
                resize_ColorRGBs(colors, clength);
                memcpy(colors->value, acolors->value, clength);
            }*/
            for (int k = 0; k < acolors->length; k++) {
                color_rgb acolor = acolors->value[k];

                // If not in list
                byte inlist = 0;
                for (int l = 0; l < colors->length; l++) {
                    if (color_rgb_equal(colors->value[l], acolor)) {
                        inlist = 1;
                        break;
                    }
                }

                if (inlist) {
                    continue;
                }

                add_to_ColorRGBs(colors, acolor);
            }

            // NOTE: For now lets assume same depth
            // add aoctree into voctree
            // account for depth difference
            // position, size for placement into new grid?
            // combine colors too

            byte3 position;
            for (position.x = vposition.x; position.x < vposition.x + vsize.x; position.x++) {
                for (position.y = vposition.y; position.y < vposition.y + vsize.y; position.y++) {
                    for (position.z = vposition.z; position.z < vposition.z + vsize.z; position.z++) {

                        byte3 gposition = (byte3) {
                            position.x - vposition.x,
                            position.y - vposition.y,
                            position.z - vposition.z
                        };

                        byte value = get_value_VoxelNode(aoctree, ndepth->value, gposition, 0);

                        if (!value) {
                            continue;
                        }

                        // TODO: Use lookups later
                        // Convert to our placement
                        // get color from value
                        color_rgb pcolor = acolors->value[value - 1];
                        // find color in place vox

                        for (int k = 0; k < colors->length; k++) {
                            if (color_rgb_equal(colors->value[k], pcolor)) {
                                value = k + 1;  // + 1 for air
                                break;
                            }
                        }

                        set_VoxelNode(voctree, ndepth->value, position, value, 0);

                        if (position.x >= new_csize.x) new_csize.x = position.x;
                        if (position.y >= new_csize.y) new_csize.y = position.y;
                        if (position.z >= new_csize.z) new_csize.z = position.z;

                        // zox_log("At [%ix%ix%i] placed [%i] - from [%ix%ix%i]", position.x, position.y, position.z, value, gposition.x, gposition.y, gposition.z);
                    }
                }
            }

            // zox_log(" + vox %i [%s] c[%i] at [%ix%ix%i] of s[%ix%ix%i]", j, zox_get_name(vox), acolors->length, vposition.x, vposition.y, vposition.z, vsize.x, vsize.y, vsize.z);
        }

        write_unlock_VoxelNode(voctree);

        // bscale->value = get_chunk_scale(ndepth->value, tdepth, terrain_scalev);
        csize->value = new_csize;
        bscale->value = (1.0f / (2 * powers_of_two_byte[ndepth->value]));

        dirty->value = zox_dirty_trigger;

        // zox_sys_e();
        // zox_set(e, MaxRenderDepth, { ndepth->value });
        // zox_set(e, ChunkLod, { 0 });
        // zox_set(e, RenderDepth, { ndepth->value });

        /*zox_log(" - colors [%i]", colors->length);
        zox_log(" - vdepth [%i] - grid max [%i]", ndepth->value, powers_of_two_byte[ndepth->value]);
        zox_log(" - csize [%ix%ix%i]", csize->value.x, csize->value.y, csize->value.z);
        zox_log(" - bscale [%f]", bscale->value);*/
    }
} zox_sys_end(CombineVoxSystem);
