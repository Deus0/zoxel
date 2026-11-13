

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
    // NOTE: Texture bakes from the voxels, reduced to model_depth
    entity vox = get_model_vox(world, model_base);
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
        model,
        texture,
        name,
        slot);
    zox_set_unique_name(e2, vox_name);
    zox_setv(e2, MaxRenderDepth, model_depth);
    return e2;
}

// NOTE: Spawns Gear from different vox files
void equips_realm_spawn_system(iter* it) {
    zox_sys_on_begin();
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
            entity hatty = spawn_realm_model_item_filename(
                world,
                e,
                "Hatty",
                "hatty",
                zox_slot_hat);
            zox_add(hatty, Hat);
        }
        {
            entity fancyhat = spawn_realm_model_item_filename(
                world,
                e,
                "Magician Hat",
                "cavalier_hat",
                zox_slot_hat);
            zox_add(fancyhat, Hat);
        }
        {
            entity tophat = spawn_realm_model_item_filename(
                world,
                e,
                "Top Hat",
                "top_hat",
                zox_slot_hat);
            zox_add(tophat, Hat);
        }
        // a shirt, generated from a blueprint like body parts (doc/body_equips.md)
        {
            // the chest's blueprint scale, the biggest of the realm's two chests (gam/zoxel/sys/body_parts.c)
            const float3 chest_scale = { 0.54f, 0.54f, 0.56f };
            const byte3 chest_size = byte3_scale3f(byte3_single(octree_size(nodegraph_max_depth)), chest_scale);
            const lint shirt_seed = zox_getv(e, Seed) * 4441;
            const color cloth = color_rgb_to_color(seed_to_color_rgb_range(shirt_seed,
                (float2) { 0, 360 }, (float2) { 35, 65 }, (float2) { 35, 55 }));
            const color trim = color_rgb_to_color(seed_to_color_rgb_range(shirt_seed + 1,
                (float2) { 0, 360 }, (float2) { 20, 50 }, (float2) { 15, 30 }));
            const entity blueprint = spawn_blueprint_shirt(world, chest_size, nodegraph_max_depth, cloth, trim);
            // the chest's depth (body_parts.c), so its voxels line up
            const byte model_depth = block_depth + 1;
            const entity shirt = spawn_realm_item_equip_blueprint(world, e, shirt_seed, model_depth,
                "Shirt", zox_slot_shirt, blueprint);
            zox_add(shirt, Shirt);
        }
        zox_logv("Realm [bodys] [X] spawned.");
    }
    zox_sys_on_end();
} zoxd_system(equips_realm_spawn_system);
