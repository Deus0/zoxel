

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

// the size in body voxels of a realm part, from its blueprint scale (gam/zoxel/sys/body_parts.c)
//  - a fill spans its blueprint size and one, parts at the limbs' depth (block_depth) are half that, the chest's all of it
static inline byte3 body_part_voxels(float3 scale, byte half) {
    const byte3 size = byte3_add(byte3_scale3f(byte3_single(octree_size(nodegraph_max_depth)), scale), byte3_single(1));
    return half ? (byte3) { (size.x + 1) / 2, (size.y + 1) / 2, (size.z + 1) / 2 } : size;
}

// the realm's cloth and trim colours for a piece of gear, from its seed
static inline void realm_gear_colors(lint seed, color* cloth, color* trim) {
    *cloth = color_rgb_to_color(seed_to_color_rgb_range(seed,
        (float2) { 0, 360 }, (float2) { 35, 65 }, (float2) { 35, 55 }));
    *trim = color_rgb_to_color(seed_to_color_rgb_range(seed + 1,
        (float2) { 0, 360 }, (float2) { 20, 50 }, (float2) { 15, 30 }));
}

// cubey gear, from blueprints at the chest's depth (body_parts.c), so its voxels line up with the body's
static inline entity spawn_realm_gear(ecs* world, entity realm, lint seed, const char* name, byte slot_type, entity blueprint) {
    return spawn_realm_item_equip_blueprint(world, realm, seed, block_depth + 1, name, slot_type, blueprint);
}

// a realm shirt from a blueprint: margin its looseness over the chest, sleeve whether it has sleeves
entity spawn_realm_shirt(ecs* world, entity realm, const char* name, lint seed_shift, byte margin, byte sleeve) {
    const byte3 chest_nsize = byte3_scale3f(byte3_single(octree_size(nodegraph_max_depth)), player_chest_scale);
    chest_blob blobs[chest_blobs_count];
    get_chest_blobs(chest_nsize, blobs);
    const byte chest_width = 2 * blobs[2].radius.x + 1;
    const byte chest_height = blobs[1].center.y + blobs[1].radius.y + 1;
    // the shoulders, where they sit on it (character_body_spawn_system): centred, then up and dug in
    const byte3 shoulder_size = body_part_voxels((float3) { 0.6f, 0.4f, 0.4f }, 1);
    const byte shoulder_y = body_to_gear_blueprint(body_shoulder_offset_y()) + (chest_height - shoulder_size.y) / 2;
    // out past the chest to the shoulder's tip (capped, the shirt fits in its blueprint)
    const byte dig = body_to_gear_blueprint(body_shoulder_dig());
    const byte max_reach = (octree_size(nodegraph_max_depth) - chest_width - 2 * margin) / 2;
    const byte reach = !sleeve ? 0 : shoulder_size.x - dig < max_reach ? shoulder_size.x - dig : max_reach;
    // the upper arm hangs under the shoulder's tip: centred on it, its PartOffset out
    const byte3 bicep_size = body_part_voxels((float3) { 0.36f, 0.52f, 0.36f }, 1);
    const int arm_x = (shoulder_size.x - bicep_size.x) / 2 - body_to_gear_blueprint(1);
    const byte drop = bicep_size.y / 3;
    const lint seed = zox_getv(realm, Seed) * seed_shift;
    color cloth, trim;
    realm_gear_colors(seed, &cloth, &trim);
    const entity blueprint = spawn_blueprint_shirt(world, chest_nsize, shoulder_size, shoulder_y, nodegraph_max_depth,
        cloth, trim, margin, reach, arm_x > 0 ? arm_x : 0, bicep_size.x, drop);
    const entity shirt = spawn_realm_gear(world, realm, seed, name, zox_slot_shirt, blueprint);
    zox_add(shirt, Shirt);
    return shirt;
}

// realm shorts: over the hips, a leg down over each thigh (zox_shorts_leg long, the shorts slot's PartOffset)
entity spawn_realm_shorts(ecs* world, entity realm, const char* name, lint seed_shift, byte margin) {
    const byte3 hips_size = body_part_voxels((float3) { 0.92f, 0.4f, 0.6f }, 1);
    const byte3 thigh_size = body_part_voxels((float3) { 0.42f, 0.76f, 0.42f }, 1);
    // the thighs' corners in the hips (bottom anchor, their PartOffset), a leg's corner is its thigh's in the shorts
    const byte thigh_offset = body_to_gear_blueprint(octree_size(block_depth) / 4);
    const byte thigh_x = (hips_size.x - thigh_size.x) / 2;
    const byte2 legs_x = { thigh_x - thigh_offset, thigh_x + thigh_offset };
    const lint seed = zox_getv(realm, Seed) * seed_shift;
    color cloth, trim;
    realm_gear_colors(seed, &cloth, &trim);
    const entity blueprint = spawn_blueprint_shorts(world, hips_size, thigh_size, legs_x, nodegraph_max_depth,
        cloth, trim, margin, zox_shorts_leg);
    const entity shorts = spawn_realm_gear(world, realm, seed, name, zox_slot_shorts, blueprint);
    zox_add(shorts, Shorts);
    return shorts;
}

// realm shoes: one shoe, worn on each foot, up all but its top voxel (the ankle shows)
entity spawn_realm_shoes(ecs* world, entity realm, const char* name, lint seed_shift, byte margin) {
    const byte3 foot_size = body_part_voxels((float3) { 0.36f, 0.42f, 0.86f }, 1);
    const byte height = foot_size.y - 1;
    const lint seed = zox_getv(realm, Seed) * seed_shift;
    color cloth, trim;
    realm_gear_colors(seed, &cloth, &trim);
    const entity blueprint = spawn_blueprint_shoe(world, foot_size, nodegraph_max_depth, cloth, trim, margin, height);
    const entity shoes = spawn_realm_gear(world, realm, seed, name, zox_slot_shoes, blueprint);
    zox_add(shoes, Shoes);
    return shoes;
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
        // cubey gear, generated from blueprints like body parts (doc/body_equips.md): the first of each is worn
        spawn_realm_shirt(world, e, "Shirt", 4441, 0, 1);   // with sleeves over the shoulders
        spawn_realm_shirt(world, e, "Vest", 5557, 0, 0);    // sleeveless
        spawn_realm_shorts(world, e, "Shorts", 6673, 0);
        spawn_realm_shoes(world, e, "Shoes", 7789, 1);
        zox_logv("Realm [bodys] [X] spawned.");
    }
    zox_sys_on_end();
} zoxd_system(equips_realm_spawn_system);
