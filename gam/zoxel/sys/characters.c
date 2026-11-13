// Spawns a realm's characters as prefabs (npc kinds), with their stats
//  - the player's come from the same helper, fill_character_base_stats, when it spawns (player_character_stats_system)
//  - every character's stats are made here once, instances copy them (no stats spawned per npc)
void character3_realm_spawn_system(iter* it) {
    zox_sys_on_begin();
    byte is_add_stats = 1;
    byte character_depth = block_depth_limits.y;
    uint seed_shift = 36936;
    char character_name[64];
    // uint inner_seed_shift = 33;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Seed);
    zox_sys_out(CharactersChanceMax);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Seed, seed);
        zox_sys_o(CharactersChanceMax, chance);
        if (state->value != zox_generate_realm_characters) {
            continue;
        }
        uint character_seed = seed->value + seed_shift;
        byte chance_max = 0;
        entity prefab_character = prefab_character3_npc;
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
            link_hit_sound(world, e2, e, &hit_sound_flesh);
            if (is_add_stats) {
                fill_character_base_stats(world, e2, e, 16, 42, 0);
            }
            // Generate Name
            char name[64];
            generate_name(character_seed, name);
            set_ZoxName(world, e2, name);
            // Links
            chance_max += chance;
            zox_link(world, e, CharacterLinks, e2);
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
            zox_set_link(world, e2, ModelLink, model);
            zox_add(e2, CharacterSlime);
            link_hit_sound(world, e2, e, &hit_sound_slime);
            // NOTE: Unique characters clone their model, otherwise they instance it
            if (!is_characters_instanced) {
                zox_add(e2, CharacterUnique);
            }
            // Generate Name
           // set_ZoxName(world, e2, name);
            // Links
            zox_link(world, e, CharacterLinks, e2);
            chance_max += chance;
            // NOTE: This crashes it
            // ecs_defer_end(world);
            if (is_add_stats) {
                fill_character_base_stats(world, e2, e, 2, 8, 0);
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
            zox_set_link(world, e2, ModelLink, model);
            link_hit_sound(world, e2, e, strcmp(name, "slime") ? &hit_sound_flesh : &hit_sound_slime);
            if (!is_characters_instanced) {
                zox_add(e2, CharacterUnique);
            }
            chance_max += chance;
            float soul_value = (float)(souls[j]);
            float health = (float)(healths[j]);
            if (is_add_stats) {
                fill_character_base_stats(world, e2, e, soul_value, health, 0);
            }
            zox_link(world, e, CharacterLinks, e2);
            character_seed += seed_shift;
        }
        chance->value = chance_max;
        zox_logv("Realm [characters] [X] spawned");
    }
    zox_sys_on_end();
} zoxd_system(character3_realm_spawn_system);
