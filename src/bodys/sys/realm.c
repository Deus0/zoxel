
void delayed_texture_spawn(ecs* world, entity e) {
    if (zox_valid(e)) {
        zox_set(e, GenerateTexture, { zox_dirty_trigger });
    }
}

// NOTE: Confusing AF atm, nodegraphs use a set size, 32 atm, however the vox models spawn at any size, and fill gets scaled to those
//      - so we have two sizes created per model

zox_sys2(BodysRealmSpawnSystem) {
    byte nodegraph_vlength = powers_of_two[nodegraph_max_depth];

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(ModelLinks);
    zox_sys_out(ItemLinks);
    zox_sys_out(NodegraphLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(ModelLinks, models);
        zox_sys_o(ItemLinks, items);
        zox_sys_o(NodegraphLinks, graphs);

        if (state->value != zox_generate_realm_items) {
            continue;
        }

        byte mdepth = block_vox_depth + 1;

        if (mdepth >= model_lods_max_length) {
            zox_logw("[BodysRealmSpawnSystem] Does not support depth [%i] max is [%i]", mdepth, model_lods_max_length);
            continue;
        }

        zox_log("mdepth in body parts gen [%i]", mdepth);

        byte vlength = powers_of_two_byte[mdepth];
        int2 texture_size = int2_single(vlength);

        // Chest
        {
            byte variants_count = 1;

            // max size for the chest
            byte3 nsize = (byte3) {
                nodegraph_vlength / 2,
                (7 * nodegraph_vlength) / 10,
                nodegraph_vlength / 2
            };

            // colors node
            color mcolor = (color) { 200, 155, 133, 255 };
            entity node_0 = spawn_node_model_colors(world, mcolor, 1);
            add_to_NodegraphLinks(graphs, node_0);

            // fill node
            {
                // first fill is a blob pillar
                byte3 nsize_1 = (byte3) {
                    (4 * nsize.x) / 5,
                    nsize.y,
                    (4 * nsize.z) / 5,
                };
                byte3 nposition_1 = (byte3) {
                    nsize.x / 2,
                    nsize_1.y / 2,
                    nsize.z / 2
                };

                // Upper Chest Blob: spans the shoulder joints part
                byte3 nsize_2 = (byte3) {
                    nsize.x,
                    1 + nsize.y / 4,
                    nsize.z
                };
                byte3 nposition_2 = (byte3) {
                    nsize.x / 2,
                    ((7 * nsize.y) / 8) - nsize_2.y / 2,
                    nsize.z / 2
                };

                // Create our nodes

                entity node_1 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_1, nsize_1, 1);
                new_link_single_node(world, node_0, node_1);

                entity node_2 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_2, nsize_2, 1);
                new_link_single_node(world, node_1, node_2);
            }

            zox_make_neww(model_group);
            zox_set_unique_name(model_group, "bodys_chest");
            zox_add_tag(model_group, BodyModel);
            add_to_ModelLinks(models, model_group);

            entity texture_model = 0;
            ModelLinks variants = (ModelLinks) { 0 };
            lint bseed = 888 * i;
            for (byte j = 0; j < variants_count; j++) {
                lint vseed = bseed + j * 1209;
                color vcolor = (color) { 200, 200, 155, 255 };

                byte3 vsize = (byte3) {
                    vlength / 2,
                    (7 * vlength) / 10,
                    vlength / 2
                };

                ModelLods mlods2 = (ModelLods) { };
                entity mlods = spawn_model_lods(world, vcolor, vseed, mdepth, vsize, "mchest", &mlods2);
                zox_set_unique_name(mlods, "bodys_chest_mlods");
                add_to_ModelLinks(&variants, mlods);

                if (j == 0) {
                    texture_model = mlods2.value[mdepth];
                }

                entity process = spawn_process_model(world, prefab_process_model, node_0, mlods);
            }

            zox_set_ptr(model_group, ModelLinks, variants);

            // entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("playerer"));
            entity model = variants.value[0]; //  model_group;

            // TODO: Generate based on model
            // entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("taskbar_body"));

            // Spawn Item Texture
            entity texture = spawn_texture(world, prefab_vox_texture, texture_size);
            zox_set_name_e(texture, "texture_mchest");
            zox_set(texture, VoxBakeSide, { direction_front });
            // direction_front });
            zox_set(texture_model, TextureLink, { texture });
            // Link Model to Texture
            // zox_set_unique_name(texture_model, "bodys_chest_model_high");
            zox_set(texture, VoxLink, { texture_model });
            // TODO: Spawn Texture with Model Graph
            delay_event(world, &delayed_texture_spawn, texture, 1.0f);

            entity ritem = spawn_item_body(world, model, texture, "Chest");
            zox_set(ritem, SlotType, { zox_slot_core });
            add_to_ItemLinks(items, ritem);
        }

        // Head
        {
            byte variants_count = 1;

            byte3 nsize = byte3_single(1 + nodegraph_vlength / 4);

            color skin_color = (color) { 230, 155, 133, 255 };
            entity e1 = spawn_node_model_colors(world, skin_color, 1);
            add_to_NodegraphLinks(graphs, e1);
            {
                byte3 nposition_1 = byte3_half(nsize);

                entity e2 = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, nposition_1, nsize, 1);
                new_link_single_node(world, e1, e2);

                // spawn eyes

                byte eye_place_type = zox_model_node_paint; // fill | paint;

                byte eye_ridge = nsize.x / 6;
                if (eye_ridge < 0) eye_ridge = 1;
                byte eye_size = nsize.x / 5;
                if (eye_size < 0) eye_size = 1;
                byte half_eye_size = eye_size / 2;
                if (half_eye_size == 0) half_eye_size = 1;

                byte eye_pos_y = 1 + nsize.y / 2;
                byte eye_pos_z = nsize.z - half_eye_size;

                // eye_size++;
                if (eye_place_type == zox_model_node_paint) {
                    eye_pos_z -= half_eye_size;
                }

                byte3 leye_position = (byte3) {
                    nsize.x / 2 - half_eye_size - eye_ridge,
                    eye_pos_y,
                    eye_pos_z
                };
                byte3 reye_position = (byte3) {
                    nsize.x / 2 + half_eye_size + eye_ridge,
                    eye_pos_y,
                    eye_pos_z
                };
                //  + nsize.x % 2
                if (eye_place_type == zox_model_node_paint) {
                    // adjust for centered
                    // leye_position.x += half_eye_size;
                    reye_position.x -= half_eye_size;
                }

                zox_log("=> eye - ridge [%i] size [%i] at [(L:%ixR:%i)x%ix%i] nsize[%ix%ix%i]", eye_ridge, eye_size, leye_position.x, reye_position.x, eye_pos_y, eye_pos_z, nsize.x, nsize.y, nsize.z);

                color eye_color = (color) { 55, 200, 99, 255 };
                entity e3 = spawn_node_model_colors(world, eye_color, 1);
                new_link_single_node(world, e2, e3);

                entity e4 = spawn_node_model_at(world, prefab_node_model, eye_place_type, leye_position, byte3_single(eye_size), 2);
                new_link_single_node(world, e3, e4);

                entity e5 = spawn_node_model_at(world, prefab_node_model, eye_place_type, reye_position, byte3_single(eye_size), 2);
                new_link_single_node(world, e4, e5);
            }

            zox_make_neww(model_group);
            zox_set_unique_name(model_group, "bodys_model_group_head");
            zox_add_tag(model_group, BodyModel);
            add_to_ModelLinks(models, model_group);

            entity texture_model = 0;
            ModelLinks variants = (ModelLinks) { 0 };
            lint bseed = 888 * i;
            for (byte j = 0; j < variants_count; j++) {
                lint vseed = bseed + j * 1209;
                color vcolor = (color) { 200, 200, 155, 255 };

                byte3 vsize = byte3_single(1 + vlength / 4);

                ModelLods mlods2 = (ModelLods) { };
                entity mlods = spawn_model_lods(world, vcolor, vseed, mdepth, vsize, "bodys_model_head", &mlods2);
                zox_set_unique_name(mlods, "bodys_mlods_head");
                add_to_ModelLinks(&variants, mlods);

                if (j == 0) {
                    texture_model = mlods2.value[mdepth];
                }

                entity process = spawn_process_model(world, prefab_process_model, e1, mlods);
            }

            zox_set_ptr(model_group, ModelLinks, variants);

            // entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("playerer"));
            entity model = variants.value[0]; //  model_group;

            // TODO: Generate based on model
            // entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("taskbar_body"));

            // Spawn Item Texture
            entity texture = spawn_texture(world, prefab_vox_texture, texture_size);
            zox_set_name_e(texture, "bodys_texture_head");
            zox_set(texture, VoxBakeSide, { direction_front });
            // direction_front });
            zox_set(texture_model, TextureLink, { texture });
            // Link Model to Texture
            // zox_set_unique_name(texture_model, "bodys_chest_model_high");
            zox_set(texture, VoxLink, { texture_model });
            // TODO: Spawn Texture with Model Graph
            delay_event(world, &delayed_texture_spawn, texture, 1.0f);

            entity ritem = spawn_item_body(world, model, texture, "Head");
            zox_set(ritem, SlotType, { zox_slot_head });
            add_to_ItemLinks(items, ritem);
        }

        zox_logv("At [%f] Realm [bodys] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(BodysRealmSpawnSystem);
