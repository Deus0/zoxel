entity get_lodded_model(ecs* world, entity e, byte depth) {
    if (!zox_valid(e) || !zox_has(e, ModelLods) || !zox_has(e, MaxRenderDepth)) {
        zox_loge("Item has Invalid Model [%s]", zox_get_name(e));
        return 0;
    }
    byte mdepth = zox_getv(e, MaxRenderDepth);
    if (depth > mdepth) {
        depth = mdepth;
    }
    zox_geter(e, ModelLods, mlods);
    return mlods->value[depth];
}

// NOTE: Something disposes of Model when Realm Dies
//  - We clone the model here otherwise it crashes on second Realm spawn (reload game)
entity spawn_realm_model_item_filename(ecs* world, entity realm, const char* name, const char* vox_name, byte slot) {
    entity model_base = string_hashmap_get(files_hashmap_voxes, new_string_data(vox_name));
    if (!zox_valid(model_base)) {
        zox_log("Model [%s] Not Found", vox_name);
        return 0;
    }
    if (!zox_has(model_base, MaxRenderDepth)) {
        zox_log("Model [%s] has no MaxRenderDepth", vox_name);
        return 0;
    }
    byte model_depth = zox_getv(model_base, MaxRenderDepth);
    if (block_vox_depth < model_depth) {
        model_depth = block_vox_depth;
    }
    entity vox = get_lodded_model(world, model_base, model_depth);
    if (!zox_valid(vox)) {
        zox_log("Model [%s] has no Vox Model", vox_name);
        return 0;
    }
    // clone model
    entity model = zox_ins(world, model_base);
    zox_set_parent(world, model, realm);
    // entity model = model_base;
    //entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("hatty"));
    // Spawn Texture
    short length = octree_size(model_depth);
    entity texture = spawn_texture_from_vox(world, vox, byte2_single(length));
    zox_set_unique_name(texture, vox_name);
    zox_set_parent(world, texture, realm);
    // Spawn Item
    entity e2 = spawn_realm_item_equip(world, realm, model, texture, name, slot);
    zox_set_unique_name(e2, vox_name);
    zox_set(e2, MaxRenderDepth, { model_depth });
    return e2;
}

// NOTE: Spawns Gear from different vox files
zox_sys2(EquipsRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    // zox_sys_out(ModelLinks);
    zox_sys_out(ItemLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        // zox_sys_o(ModelLinks, models);
        zox_sys_o(ItemLinks, items);
        if (state->value != zox_generate_realm_items) {
            continue;
        }
        {
            entity e2 = spawn_realm_model_item_filename(world, e, "Hatty", "hatty", zox_slot_hat);
            add_to_ItemLinks(items, e2);
            // zox_log("model %s", zox_get_name(model));
        }
        {
            entity e2 = spawn_realm_model_item_filename(world, e, "Magician Hat", "cavalier_hat", zox_slot_hat);
            add_to_ItemLinks(items, e2);
            // zox_log("model %s", zox_get_name(model));
        }
        {
            entity e2 = spawn_realm_model_item_filename(world, e, "Top Hat", "top_hat", zox_slot_hat);
            add_to_ItemLinks(items, e2);
            // zox_log("model %s", zox_get_name(model));
        }
        zox_logv("At [%f] Realm [bodys] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(EquipsRealmSpawnSystem);

            /*entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("hatty"));
            byte model_depth = zox_getv(model, MaxRenderDepth);
            if (block_vox_depth < model_depth) {
                model_depth = block_vox_depth;
            }
            //entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("hatty"));
            entity vox = get_lodded_model(world, model, model_depth);
            entity texture = spawn_texture_from_vox(world, vox, byte2_single(powers_of_two[model_depth]));
            zox_set_unique_name(texture, "texture_hatty");
            zox_set_parent(world, texture, e);
            entity e2 = spawn_realm_item_equip(world, e, model, texture, "Hatty", zox_slot_hat);
            zox_set_unique_name(e2, "hatty");
            zox_set(e2, MaxRenderDepth, { model_depth });*/


            // entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("top_hat"));
            /*entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("top_hat"));
            byte model_depth = zox_getv(model, MaxRenderDepth);
            if (block_vox_depth < model_depth) {
                model_depth = block_vox_depth;
            }
            entity vox = get_lodded_model(world, model, model_depth);
            entity texture = spawn_texture_from_vox(world, vox, byte2_single(powers_of_two[model_depth]));
            zox_set_unique_name(texture, "texture_top_hat");
            zox_set_parent(world, texture, e);
            entity e2 = spawn_realm_item_equip(world, e, model, texture, "Top Hat", zox_slot_hat);
            zox_set_unique_name(e2, "top_hat");
            zox_set(e2, MaxRenderDepth, { model_depth });*/
