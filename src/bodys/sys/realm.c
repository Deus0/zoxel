
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
            entity blueprint_chest = spawn_blueprint_chest(world, nsize);
            add_to_NodegraphLinks(graphs, blueprint_chest);

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

                entity process = spawn_process_model(world, prefab_process_model, blueprint_chest, mlods);
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

            entity blueprint_head = spawn_blueprint_head(world, nsize);
            add_to_NodegraphLinks(graphs, blueprint_head);

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

                entity process = spawn_process_model(world, prefab_process_model, blueprint_head, mlods);
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
