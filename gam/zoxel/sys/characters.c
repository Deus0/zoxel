entity spawn_model_nodegraph_slime(
    ecs* world,
    entity parent,
    byte depth,
    byte3 size,
    color skin_color,
    color eye_color)
{
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
    entity root_node = spawn_node_model_colors(world, skin_color, 1);
    zox_set_parent(world, root_node, parent);
    entity colors_node2 = spawn_node_model_colors(world, eye_color, 1);
    new_link_single_node(world, root_node, colors_node2);
    entity fill_node = spawn_node_model_at(world, prefab, zox_model_node_fill, core_position, size, 1);
    zox_set(fill_node, NodeDepth, { depth });
    new_link_single_node(world, colors_node2, fill_node);
    entity left_eye_node = spawn_node_model_at(world, prefab, zox_model_node_paint, leye_position, eye_size, 2);
    new_link_single_node(world, fill_node, left_eye_node);
    entity right_eye_node = spawn_node_model_at(world, prefab, zox_model_node_paint, reye_position, eye_size, 2);
    new_link_single_node(world, left_eye_node, right_eye_node);
    return root_node;
}

entity spawn_model_slime(ecs* world, entity parent, lint seed, byte depth, byte variants_count) {
    int seed_shift = 33;
    lint variant_seed_step = 1209;
    float squash = randf_range(0.7f, 0.9f);
    float3 ratio = (float3) { squash, 1.0f, squash };
    color skin_color = seed_color(&seed, seed_shift);
    color eye_color = seed_color(&seed, seed_shift);
    // Our nodegraph
    entity nodegraph = spawn_model_nodegraph_slime(
        world,
        parent,
        nodegraph_max_depth,
        get_scaled_size(nodegraph_max_depth, ratio),
        skin_color,
        eye_color);
    zox_set_parent(world, nodegraph, parent);
    // zox_make_neww(model_group);
    entity model_group = zox_ins(world, prefab_model_group);
    zox_set_parent(world, model_group, parent);
    zox_set_unique_name(model_group, "model_group_slime");
    // Spawn variant models with the blueprint
    ModelLinks variants = { 0 };
    byte3 variant_size = get_scaled_size(depth, ratio);
    for (byte i = 0; i < variants_count; ++i) {
        lint variant_seed = seed + (lint) i * variant_seed_step;
        ModelLods lods = { 0 };
        entity model = spawn_model_lods(
            world,
            model_group,
            prefab_vox,
            color_red,
            variant_seed,
            depth,
            variant_size,
            "lod_slime",
            &lods);
        zox_set_unique_name(model, "model_slime");
        add_to_ModelLinks(&variants, model);
        spawn_process_model(world, prefab_process_model, nodegraph, model);
    }
    zox_set_ptr(model_group, ModelLinks, variants);
    return model_group;
}

// TODO: Just spawn characters here linked to realm, used as prefabs later
zox_sys2(Character3RealmSpawnSystem) {
    byte character_depth = block_vox_depth_limits.y;
    uint seed_shift = 36936;
    uint inner_seed_shift = 33;
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
        entity prefab_character = is_characters_instanced ? prefab_character3_instanced_npc : prefab_character3_npc;
        // add files
        entity rsoul = zox_get_child_by_id(world, e, zox_id(StatSoul));
        entity rhealth = zox_get_child_by_id(world, e, zox_id(StatHealth));
        // add our skeleton prefab
        if (!zox_no_humanoids)
        {
            byte chance = 5;
            entity e2 = spawn_character3_meta(world, prefab_character3_skeleton_npc, character_seed, "Boney", chance);
            zox_set_parent(world, e2, e);
            spawn_stat_level(world, e2, rsoul, 5);
            spawn_stat_state(world, e2, rhealth, 21, 21);
            chance_max += chance;
            add_to_CharacterLinks(characters, e2);
            character_seed += seed_shift;
        }
        // SLIMES
        // TODO: Spawn one blueprint and just use that
        byte slime_types = 4;
        byte variants = 1;
        byte generated_chance = 65 / slime_types;
        for (int j = 0; j < slime_types; j++) {

            /*lint inner_character_seed = character_seed;
            float squash = seed_range(inner_character_seed, 0.6f, 0.9f);
            inner_character_seed += inner_seed_shift;
            float3 ratio = (float3) { squash, 1.0f, squash };
            color skin_color = seed_color(&inner_character_seed, inner_seed_shift);
            color eye_color = seed_color(&inner_character_seed, inner_seed_shift);
            byte3 nodegraph_size = get_scaled_size(nodegraph_max_depth, ratio);
            entity nodegraph = spawn_model_nodegraph_slime(
                world,
                e,
                nodegraph_max_depth,
                nodegraph_size,
                skin_color,
                eye_color);
            byte3 model_size = get_scaled_size(character_depth, ratio);
            entity model = spawn_blueprint_models(
                world,
                e,
                nodegraph,
                character_seed,
                "slime",
                character_depth,
                model_size,
                variants
            );*/

            entity model = spawn_model_slime(world, e, character_seed, character_depth, variants);

            if (!zox_valid(model)) {
                zox_loge("realm has invalid model [%i]", j);
                continue;
            }
            zox_add_tag(model, ModelCharacter);
            byte chance = generated_chance;
            entity e2 = spawn_character3_meta(world, prefab_character, character_seed, "character", chance);
            zox_setv(e2, ModelLink, model);
            zox_add_tag(e2, CharacterGeneric);
            zox_set_parent(world, e2, e);
            add_to_CharacterLinks(characters, e2);
            chance_max += chance;
            spawn_stat_level(world, e2, rsoul, 2);
            spawn_stat_state(world, e2, rhealth, 8, 8);
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
            entity e2 = spawn_character3_meta(world, prefab_character, character_seed, name, chance);
            zox_set_name(e2, name); // assuming name is unique to the ecs world
            zox_setv(e2, ModelLink, model);
            chance_max += chance;
            float soul_value = (float)(souls[j]);
            float health = (float)(healths[j]);
            spawn_stat_level(world, e2, rsoul, soul_value);
            spawn_stat_state(world, e2, rhealth, health, health);
            add_to_CharacterLinks(characters, e2);
            character_seed += seed_shift;
        }
        chance->value = chance_max;
        zox_logv("At [%f] Realm [characters] [%i] spawned.", zox_current_time, characters->length);
    }
} zox_sys_end(Character3RealmSpawnSystem);
