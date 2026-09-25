// TODO: Just spawn characters here linked to realm, used as prefabs later
zox_sys2(Character3RealmSpawnSystem) {
    byte is_add_stats = 1;
    byte character_depth = block_depth_limits.y;
    uint seed_shift = 36936;
    char character_name[64];
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
        entity base_soul = zox_get_child_by_id(
            world,
            e,
            zox_id(StatSoul));
        entity base_health = zox_get_child_by_id(
            world,
            e,
            zox_id(StatHealth));
        // add our skeleton prefab
        if (!zox_no_humanoids) {
            byte chance = 2;
            generate_name(character_seed, character_name);
            entity e2 = spawn_character3_meta(
                world,
                prefab_character3_skeleton_npc,
                e,
                character_seed,
                character_name, // "Boney",
                chance);
            if (!e2) {
                zox_loge("Boney failed to spawn");
                continue;
            }
            zox_add(e2, QuestGiver);
            if (is_add_stats) {
                spawn_stat_level(world, e2, base_soul, 5);
                spawn_stat_state(world, e2, base_health, 21, 21);
            }
            // Generate Name
            char name[64];
            generate_name(character_seed, name);
            set_ZoxName(world, e2, name);
            // Links
            chance_max += chance;
            add_to_CharacterLinks(characters, e2);
            character_seed += seed_shift;
        }
        // SLIMES
        // TODO: Spawn one blueprint and just use that
        byte slime_types = 1;
        byte variants = 4;
        byte generated_chance = slime_types > 0 ?
            65 / slime_types :
            0;
        entity slime_nodegraph = spawn_model_nodegraph_slime(world, e);
        //entity cookie_nodegraph = spawn_model_nodegraph_cookie(world, e);
        for (int j = 0; j < slime_types; j++) {
            // lint inner_character_seed = character_seed;
            entity nodegraph = slime_nodegraph;
            /*if (j == 0) {
                nodegraph = cookie_nodegraph;
            } else {
                nodegraph = slime_nodegraph;
            }*/
            entity model = spawn_blueprint_models(
                world,
                e,
                nodegraph,
                character_seed,
                "slem",
                character_depth,
                variants).x;
            if (!zox_valid(model)) {
                zox_loge("realm has invalid model [%i]",
                    j);
                continue;
            }
            zox_add(model, ModelCharacter);
            byte chance = generated_chance;
            char character_name[64];
            generate_name(character_seed, character_name);
            entity e2 = spawn_character3_meta(
                world,
                prefab_character,
                e,
                character_seed,
                character_name,
                chance);
            if (!e2) {
                zox_loge("Generated Character Failed");
                continue;
            }
            zox_setv(e2, ModelLink, model);
            zox_add(e2, CharacterGeneric);
            // Generate Name
           // set_ZoxName(world, e2, name);
            // Links
            zox_set_parent(world, e2, e);
            add_to_CharacterLinks(characters, e2);
            chance_max += chance;
            // NOTE: This crashes it
            // ecs_defer_end(world);
            if (is_add_stats) {
                spawn_stat_level(world, e2, base_soul, 2);
                spawn_stat_state(world, e2, base_health, 8, 8);
            }
            // ecs_defer_begin(world);
            character_seed += seed_shift;
        }
        // Spawn our Vox Files
        int count = 5; // count of below array
        char* vox_names[] = {
            "slime",
            "chicken",
            "mrpenguin",
            "bob",
            "bigmrpenguin"
        };
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
            // Set Names
            // zox_set_name(e2, name); // assuming name is
            // set_ZoxName(world, e2, name);
            // Links unique to the ecs world
            zox_setv(e2, ModelLink, model);
            chance_max += chance;
            float soul_value = (float)(souls[j]);
            float health = (float)(healths[j]);
            if (is_add_stats) {
                spawn_stat_level(world, e2, base_soul, soul_value);
                spawn_stat_state(world, e2, base_health, health, health);
            }
            add_to_CharacterLinks(characters, e2);
            character_seed += seed_shift;
        }
        chance->value = chance_max;
        zox_logv("Realm [characters] [X] spawned");
    }
} zox_sys_end(Character3RealmSpawnSystem);
