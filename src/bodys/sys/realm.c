zox_sys2(BodysRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(ModelLinks);
    zox_sys_out(ItemLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(ModelLinks, models);
        zox_sys_o(ItemLinks, items);

        if (state->value != zox_generate_realm_items) {
            continue;
        }

        if (!models->length) {
            zox_log_error("No Models to spawn items from")
            continue;
        }

        {
            entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("playerer"));

            entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("taskbar_body"));

            entity item = spawn_item_body(
                world,
                model,
                texture,
                "Chest"
            );

            add_to_ItemLinks(items, item);

            zox_log("model %s", zox_get_name(model));
        }


        zox_logv("At [%f] Realm [bodys] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(BodysRealmSpawnSystem);
