/*void spawn_realm_equips(ecs *world, entity realm) {
    if (!zox_has(realm, ItemLinks)) {
        zox_log_error("Realm does not have ItemLinks [%lu]", realm)
        return;
    }
    if (!zox_has(realm, ModelLinks)) {
        zox_log_error("Realm does not have ModelLinks [%lu]", realm)
        return;
    }

    zox_muter(realm, ItemLinks, items);




    zox_logv("At [%f] Realm [equips] [%i] spawned.", zox_current_time, items->length);
}*/


zox_sys2(EquipsRealmSpawnSystem) {
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
            entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("top_hat"));

            entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("top_hat"));

            entity top_hat = spawn_equip_item(
                world,
                model,
                texture,
                "Top Hat"
            );

            add_to_ItemLinks(items, top_hat);

            zox_log("model %s", zox_get_name(model));
        }

        zox_logv("At [%f] Realm [bodys] [%i] spawned.", zox_current_time, items->length);
    }
} zox_sys_end(EquipsRealmSpawnSystem);
