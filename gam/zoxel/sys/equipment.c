

// NOTE: Something disposes of Model when Realm Dies
//  - We clone the model here otherwise it crashes on second Realm spawn (reload game)
entity spawn_realm_model_item_filename(
    ecs* world,
    entity realm,
    const char* name,
    const char* vox_name,
    byte slot)
{
    entity model_base = string_hashmap_get(
        files_hashmap_voxes,
        new_string_data(vox_name));
    if (!zox_valid(model_base)) {
        zox_log("Model [%s] Not Found", vox_name);
        return 0;
    }
    byte model_depth = zox_getv(model_base, MaxRenderDepth);
    if (block_depth < model_depth) {
        model_depth = block_depth;
    }
    entity vox = get_max_model_mesh(world, model_base);
    /*if (!zox_has(model_base, MaxRenderDepth)) {
        zox_log("Model [%s] has no MaxRenderDepth", vox_name);
        return 0;
    }
    byte model_depth = zox_getv(model_base, MaxRenderDepth);
    if (block_depth < model_depth) {
        model_depth = block_depth;
    }
    entity vox = get_lodded_model(
        world,
        model_base,
        model_depth);*/
    if (!zox_valid(vox)) {
        zox_log("Model [%s] has no Vox Model", vox_name);
        return 0;
    }
    // clone model
    entity model = zox_ins(world, model_base);
    zox_set_parent(world, model, realm);
    // Spawn Texture
    short length = octree_size(model_depth);
    entity texture = spawn_texture_from_vox(
        world,
        vox,
        byte2_single(length),
        direction_front);
    zox_set_unique_name(texture, vox_name);
    zox_set_parent(world, texture, realm);
    // Spawn Item
    entity e2 = spawn_realm_item_equip(
        world,
        realm,
        model, texture, name, slot);
    zox_set_unique_name(e2, vox_name);
    zox_setv(e2, MaxRenderDepth, model_depth);
    return e2;
}

// NOTE: Spawns Gear from different vox files
zox_sys2(EquipsRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        if (state->value != zox_generate_realm_items) {
            continue;
        }
        {
            spawn_realm_model_item_filename(
                world,
                e,
                "Hatty",
                "hatty",
                zox_slot_hat);
        }
        {
            spawn_realm_model_item_filename(
                world,
                e,
                "Magician Hat",
                "cavalier_hat",
                zox_slot_hat);
        }
        {
            spawn_realm_model_item_filename(
                world,
                e,
                "Top Hat",
                "top_hat",
                zox_slot_hat);
        }
        zox_logv("Realm [bodys] [X] spawned.");
    }
} zox_sys_end(EquipsRealmSpawnSystem);
