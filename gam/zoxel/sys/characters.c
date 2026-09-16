entity spawn_model_nodegraph_cookie(
    ecs* world,
    entity parent)
{
    byte3 size = get_scaled_size(nodegraph_max_depth, float3_one);
    entity prefab = prefab_node_model;
    byte3 center = byte3_half(size);
    // Set random cookie size
    float3 size_min = (float3) { 0.55f, 0.18f, 0.55f };
    float3 size_max = (float3) { 0.95f, 0.32f, 0.95f };
    entity root_node = spawn_node_model_size(
        world,
        size_min,
        size_max);
    // Dough color
    entity dough_color = spawn_node_model_colors_seed(world, 1);
    new_link_single_node(world, root_node, dough_color);
    // Cookie body
    entity cookie_fill = spawn_node_model_at(
        world,
        prefab,
        zox_model_node_fill,
        center,
        size,
        1);
    new_link_single_node(world, dough_color, cookie_fill);
    // Chocolate chip color
    entity chocolate_color = spawn_node_model_colors_seed(world, 2);
    new_link_single_node(
        world,
        cookie_fill,
        chocolate_color);
    // A few authored chip positions for now
    byte chip_size_value = size.x / 8;
    byte3 chip_size = byte3_single(chip_size_value);
    entity chip1 = spawn_node_model_at(
        world,
        prefab,
        zox_model_node_paint,
        (byte3) {
            size.x / 3,
            size.y,
            size.z / 3
        },
        chip_size,
        2);
    new_link_single_node(world, chocolate_color, chip1);
    entity chip2 = spawn_node_model_at(
        world,
        prefab,
        zox_model_node_paint,
        (byte3) {
            size.x * 2 / 3,
            size.y,
            size.z / 2
        },
        chip_size,
        2);

    new_link_single_node(world, chip1, chip2);
    entity chip3 = spawn_node_model_at(
        world,
        prefab,
        zox_model_node_paint,
        (byte3) {
            size.x / 2,
            size.y,
            size.z * 2 / 3
        },
        chip_size,
        2);
    new_link_single_node(world, chip2, chip3);
    entity end_node = spawn_node_model(
        world,
        prefab_node_model,
        zox_model_node_end);
    new_link_single_node(world, chip3, end_node);
    zox_set_parent(world, root_node, parent);
    return root_node;
}

entity spawn_model_nodegraph_slime(ecs* world, entity parent) {
    byte3 size = get_scaled_size(nodegraph_max_depth, float3_one);
    entity prefab = prefab_node_model;
    float eye_spacing = 5.6f;
    float eye_height = 2.2f;
    byte3 core_position = byte3_half(size);
    byte eye_ridge = byte_div(size.x, eye_spacing);
    byte3 eye_size = byte3_single(byte_div(size.x, eye_spacing));
    byte eye_y = byte_div(size.y, eye_height);
    byte eye_pos_z = size.z - eye_size.z / 2;
    byte3 leye_position = (byte3) { size.x / 2 - eye_size.x - eye_ridge / 2, eye_y, eye_pos_z };
    byte3 reye_position = (byte3) { size.x / 2 + eye_ridge / 2, eye_y, eye_pos_z };
    // Nodes!!
    // Set the size
    float3 size_min = float3_single(0.52f);
    float3 size_max = float3_single(0.9f);
    entity root_node = spawn_node_model_size(world, size_min, size_max);
    // Add our colors
    entity colors_node1 = spawn_node_model_colors_seed(world, 1);
    new_link_single_node(world, root_node, colors_node1);
    entity colors_node2 = spawn_node_model_colors_seed(world, 1);
    new_link_single_node(world, colors_node1, colors_node2);
    entity fill_node = spawn_node_model_at(world, prefab, zox_model_node_fill, core_position, size, 1);
    new_link_single_node(world, colors_node2, fill_node);
    entity left_eye_node = spawn_node_model_at(world, prefab, zox_model_node_paint, leye_position, eye_size, 2);
    new_link_single_node(world, fill_node, left_eye_node);
    entity right_eye_node = spawn_node_model_at(world, prefab, zox_model_node_paint, reye_position, eye_size, 2);
    new_link_single_node(world, left_eye_node, right_eye_node);
    // Everything good must come to an end
    entity end_node = spawn_node_model(world, prefab_node_model, zox_model_node_end);
    new_link_single_node(world, right_eye_node, end_node);
    zox_set_parent(world, root_node, parent);
    return root_node;
}

// TODO: Just spawn characters here linked to realm, used as prefabs later
zox_sys2(Character3RealmSpawnSystem) {
    byte character_depth = block_depth_limits.y;
    uint seed_shift = 36936;
    // uint inner_seed_shift = 33;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Seed);
    zox_sys_out(CharacterLinks);
    zox_sys_out(CharactersChanceMax);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Seed, seed);
        zox_sys_o(CharacterLinks, characters);
        zox_sys_o(CharactersChanceMax, chance);
        if (state->value != zox_generate_realm_characters) {
            continue;
        }
        uint character_seed = seed->value + seed_shift;
        byte chance_max = 0;
        entity prefab_character = is_characters_instanced ?
            prefab_character3_instanced_npc :
            prefab_character3_npc;
        // add files
        entity rsoul = zox_get_child_by_id(
            world,
            e,
            zox_id(StatSoul));
        entity rhealth = zox_get_child_by_id(
            world,
            e,
            zox_id(StatHealth));
        // add our skeleton prefab
        if (!zox_no_humanoids) {
            byte chance = 2;
            entity e2 = spawn_character3_meta(
                world,
                prefab_character3_skeleton_npc,
                e,
                character_seed,
                "Boney",
                chance);
            if (!e2) {
                zox_loge("Boney failed to spawn");
                continue;
            }
            // spawn_stat_level(world, e2, rsoul, 5);
            // spawn_stat_state(world, e2, rhealth, 21, 21);
            chance_max += chance;
            add_to_CharacterLinks(characters, e2);
            character_seed += seed_shift;
        }
        // SLIMES
        // TODO: Spawn one blueprint and just use that
        byte slime_types = 4;
        byte variants = 1;
        byte generated_chance = slime_types > 0 ? 65 / slime_types : 0;
        entity slime_nodegraph = spawn_model_nodegraph_slime(world, e);
        entity cookie_nodegraph = spawn_model_nodegraph_cookie(world, e);
        for (int j = 0; j < slime_types; j++) {
            // lint inner_character_seed = character_seed;
            entity nodegraph;
            if (j == 0) {
                nodegraph = cookie_nodegraph;
            } else {
                nodegraph = slime_nodegraph;
            }
            entity model = spawn_blueprint_models(
                world,
                e,
                nodegraph,
                character_seed,
                "slem",
                character_depth,
                variants);
            if (!zox_valid(model)) {
                zox_loge("realm has invalid model [%i]",
                    j);
                continue;
            }
            zox_add(model, ModelCharacter);
            byte chance = generated_chance;
            entity e2 = spawn_character3_meta(
                world,
                prefab_character,
                e,
                character_seed,
                "character",
                chance);
            zox_setv(e2, ModelLink, model);
            zox_add(e2, CharacterGeneric);
            zox_set_parent(world, e2, e);
            add_to_CharacterLinks(characters, e2);
            chance_max += chance;
            // spawn_stat_level(world, e2, rsoul, 2);
            // spawn_stat_state(world, e2, rhealth, 8, 8);
            character_seed += seed_shift;
        }
        // Spawn our Vox Files
        int count = 5; // count of below array
        char* vox_names[] = { "slime", "chicken", "mrpenguin", "bob", "bigmrpenguin" };
        // 30% are premades
        byte chances[] = { 10, 10, 4, 4, 2 };
        byte souls[] = { 1, 1, 2, 1, 3 };
        byte healths[] = { 6, 4, 8, 6, 12 };
        if (mrpenguin_mode) {
            for (int j = 0; j < 5; j++) chances[j] = 0;
            chances[4] = 100;
        }
        for (int j = 0; j < count; j++) {
            const char* name = vox_names[j];
            entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(name));
            if (!zox_valid(model)) {
                continue;
            }
            // can choose here properties for spawning
            byte chance = chances[j];
            entity e2 = spawn_character3_meta(
                world,
                prefab_character,
                e,
                character_seed,
                name,
                chance);
            zox_set_name(e2, name); // assuming name is unique to the ecs world
            zox_setv(e2, ModelLink, model);
            chance_max += chance;
            float soul_value = (float)(souls[j]);
            float health = (float)(healths[j]);
            // spawn_stat_level(world, e2, rsoul, soul_value);
            // spawn_stat_state(world, e2, rhealth, health, health);
            add_to_CharacterLinks(characters, e2);
            character_seed += seed_shift;
        }
        chance->value = chance_max;
        zox_logv("Realm [characters] [X] spawned");
    }
} zox_sys_end(Character3RealmSpawnSystem);
