void spawn_realm_equips(ecs *world, const entity realm) {
    if (!zox_has(realm, ItemLinks)) {
        zox_log_error("Realm does not have ItemLinks [%lu]", realm)
        return;
    }
    if (!zox_has(realm, ModelLinks)) {
        zox_log_error("Realm does not have ModelLinks [%lu]", realm)
        return;
    }

    zox_muter(realm, ItemLinks, items);

    {
        const entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("top_hat"));
        const entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("top_hat"));

        const entity top_hat = spawn_equip_item(
            world,
            model,
            texture,
            "Top Hat"
        );
        add_to_ItemLinks(items, top_hat);
        zox_log("model %s", zox_get_name(model));
    }


    zox_logv("At [%f] Realm [equips] [%i] spawned.", zox_current_time, items->length);
}
