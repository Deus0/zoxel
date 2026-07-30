entity spawn_model_slime(ecs* world, entity parent, lint seed, byte depth, byte variants_count) {
    int seed_shift = 33;
    short length = octree_size(depth);
    byte3 size = byte3_single(length);
    // Spawn Slime
    // TODO: Unique per variant size
    // TODO: Shift Properties from Blueprint to Variant (with seed) - i.e. use Node Process Data
    /*byte graph_depth = nodegraph_max_depth;
    short graph_length = octree_size(graph_depth);
    float squash = seed_range(seed, 0.7f, 0.9f);
    float3 ratio = (float3) { squash, 1.0f, squash };
    byte3 vsize_max = byte3_scale3f(byte3_single(graph_length), ratio);
    // Colors - TODO: NOdegraph should use vox colors instead of nodegraph ones for this
    color skin_color = (color) { seed_range(seed+=seed_shift, 0, 255), seed_range(seed+=seed_shift,0, 255), seed_range(seed+=seed_shift, 0, 255), 255 };
    color eye_color = (color) { seed_range(seed+=seed_shift, 0, 255), seed_range(seed+=seed_shift,0, 255), seed_range(seed+=seed_shift, 0, 255), 255 };
    // Create a blueprint for slime
    entity nodegraph = spawn_model_nodegraph_slime(world, prefab_node_model, graph_depth, vsize_max, skin_color, eye_color);
    zox_set_parent(world, nodegraph, parent);
    // Creates a model group for our variants
    zox_make_neww(e);
    zox_set_unique_name(e, "modelv_slime");
    zox_add_tag(e, ModelCharacter);
    zox_set_parent(world, e, parent);
    // We spawn max possible depth models
    ModelLinks variants = (ModelLinks) { 0 };
    for (byte j = 0; j < variants_count; j++) {
        lint vseed = seed + j * 1209;
        color vcolor = color_red; // color_grayscale(rand_range(80, 180));
        byte3 vsize = byte3_scale3f(size, ratio);
        ModelLods mlods2 = (ModelLods) { };
        entity mlods = spawn_model_lods(world, vcolor, vseed, depth, vsize, "rslime", &mlods2);
        zox_set_unique_name(mlods, "mslime");
        zox_set_parent(world, mlods, e);
        add_to_ModelLinks(&variants, mlods);
        spawn_process_model(world, prefab_process_model, nodegraph, mlods);
    }
    zox_set_ptr(e, ModelLinks, variants);
    return e;*/
    // TODO: Unique per variant size
    // TODO: Shift Properties from Blueprint to Variant (with seed) - i.e. use Node Process Data
    // byte mdepth_character = depth;
    byte nodegraph_depth = nodegraph_max_depth; // - 1;
    short nodegraph_length = octree_size(nodegraph_depth);
    float squash = randf_range(0.7f, 0.9f);
    float3 ratio = (float3) { squash, 1.0f, squash };
    byte3 vsize_max = byte3_scale3f(byte3_single(nodegraph_length), ratio);
    // Colors
    color skin_color = (color) { seed_range(seed+=seed_shift, 0, 255), seed_range(seed+=seed_shift,0, 255), seed_range(seed+=seed_shift, 0, 255), 255 };
    color eye_color = (color) { seed_range(seed+=seed_shift, 0, 255), seed_range(seed+=seed_shift,0, 255), seed_range(seed+=seed_shift, 0, 255), 255 };
    // Create a blueprint for slime
    entity nodegraph = spawn_model_nodegraph_slime(world, prefab_node_model, nodegraph_depth, vsize_max, skin_color, eye_color);
    //add_to_NodegraphLinks(graphs, nodegraph);
    // Creates a model group for our variants
    zox_make_neww(model_group);
    zox_set_unique_name(model_group, "modelv_slime");
    zox_add_tag(model_group, ModelCharacter);
    // add_to_ModelLinks(models, model_group);
    // We spawn max possible depth models
    ModelLinks variants = (ModelLinks) { 0 };
    for (byte j = 0; j < grass_variants; j++) {
        lint vseed = seed + j * 1209;
        color vcolor = color_red; // color_grayscale(rand_range(80, 180));
        // short vlength = octree_size(depth);
        byte3 vsize = byte3_scale3f(size, ratio);
        ModelLods mlods2 = (ModelLods) { };
        entity mlods = spawn_model_lods(world, prefab_vox, vcolor, vseed, depth, vsize, "rslime", &mlods2);
        zox_set_unique_name(mlods, "mslime");
        add_to_ModelLinks(&variants, mlods);
        spawn_process_model(world, prefab_process_model, nodegraph, mlods);
    }
    zox_set_ptr(model_group, ModelLinks, variants);
    return model_group;
}

// TODO: Just spawn characters here linked to realm, used as prefabs later
zox_sys2(Character3RealmSpawnSystem) {
    byte character_depth = block_vox_depth_limits.y;
    uint seed_shift = 36936;
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
        // New NPCs
        {
            // entity e2 = spawn_character3
            // entity health = zox_get_child_by_name(realm, "health");
            // spawn_state_stat(world, e2, health);
            // set model too
        }
        uint seed_offset = seed_shift;
        byte chance_max = 0;
        entity prefab_character = is_characters_instanced ? prefab_character3_instanced_npc : prefab_character3_npc;
        // add files
        entity rsoul = zox_get_child_by_id(world, e, zox_id(StatSoul));
        entity rhealth = zox_get_child_by_id(world, e, zox_id(StatHealth));
        // add our skeleton prefab
        if (!zox_no_humanoids)
        {
            byte chance = 5;
            lint character_seed = seed->value + (seed_offset+=seed_shift);
            entity e2 = spawn_character3_meta(world, prefab_character3_skeleton_npc, character_seed, "Boney", chance);
            zox_set_parent(world, e2, e);
            spawn_stat_level(world, e2, rsoul, 5);
            spawn_stat_state(world, e2, rhealth, 21, 21);
            chance_max += chance;
            add_to_CharacterLinks(characters, e2);
        }
        // add model links with tag ModelCharacter
        byte slime_types = 4;
        byte variants = 1;
        byte generated_chance = 65 / slime_types;
        for (int j = 0; j < slime_types; j++) {
            // TODO: Just spawn models here
            lint character_seed = seed->value + (seed_offset+=seed_shift);
            entity model = spawn_model_slime(world, e, character_seed, character_depth, variants);
            //entity model = models->value[j];
            if (!zox_valid(model)) {
                zox_log_error("realm has invalid model [%i]", j);
                continue;
            }
            byte chance = generated_chance;
            entity e2 = spawn_character3_meta(world, prefab_character, character_seed, "character", chance);
            zox_setv(e2, ModelLink, model);
            zox_add_tag(e2, CharacterGeneric);
            zox_set_parent(world, e2, e);
            add_to_CharacterLinks(characters, e2);
            chance_max += chance;
            spawn_stat_level(world, e2, rsoul, 2);
            spawn_stat_state(world, e2, rhealth, 8, 8);
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
            lint character_seed = seed->value + (seed_offset+=seed_shift);
            entity e2 = spawn_character3_meta(world, prefab_character, character_seed, name, chance);
            zox_set_name(e2, name); // assuming name is unique to the ecs world
            zox_setv(e2, ModelLink, model);
            chance_max += chance;
            float soul_value = (float)(souls[j]);
            float health = (float)(healths[j]);
            spawn_stat_level(world, e2, rsoul, soul_value);
            spawn_stat_state(world, e2, rhealth, health, health);
            add_to_CharacterLinks(characters, e2);
        }
        chance->value = chance_max;
        zox_logv("At [%f] Realm [characters] [%i] spawned.", zox_current_time, characters->length);
    }
} zox_sys_end(Character3RealmSpawnSystem);
