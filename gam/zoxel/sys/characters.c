// TODO: Just spawn characters here linked to realm, used as prefabs later
zox_sys2(Character3RealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(ModelLinks);
    zox_sys_out(CharacterLinks);
    zox_sys_out(CharactersChanceMax);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(ModelLinks, models);
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
        byte chance_max = 0;
        // add files
        entity rsoul = zox_get_child_by_id(world, e, zox_id(StatSoul));
        entity rhealth = zox_get_child_by_id(world, e, zox_id(StatHealth));
        int count = 5; // count of below array
        char* vox_names[] = { "slime", "chicken", "mrpenguin", "bob", "bigmrpenguin" };
        byte chances[] = { 30, 30, 8, 8, 3 };
        byte souls[] = { 1, 1, 2, 1, 3 };
        byte healths[] = { 6, 4, 8, 6, 12 };
        entity prefab_character = is_characters_instanced ? prefab_character3_instanced_npc : prefab_character3_npc;
        for (int j = 0; j < count; j++) {
            const char* name = vox_names[j];
            entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(name));
            if (!zox_valid(model)) {
                continue;
            }
            // can choose here properties for spawning
            byte chance = chances[j];
            entity e2 = spawn_character3_meta(world, prefab_character, name, model, chance);
            zox_set_name(e2, name); // assuming name is unique to the ecs world
            add_to_CharacterLinks(characters, e2);
            chance_max += chance;
            float soul_value = (float)(souls[j]);
            float health = (float)(healths[j]);
            spawn_stat_level(world, e2, rsoul, soul_value);
            spawn_stat_state(world, e2, rhealth, health, health);
        }
        // add model links with tag ModelCharacter
        for (int j = 0; j < models->length; j++) {
            entity model = models->value[j];
            if (!zox_valid(model)) {
                zox_log_error("realm has invalid model [%i]", j);
                continue;
            }
            if (!zox_has(model, ModelCharacter)) {
                continue;
            }
            byte chance = 8;
            entity e2 = spawn_character3_meta(world, prefab_character, "character", model, chance);
            zox_add_tag(e2, CharacterGeneric);
            add_to_CharacterLinks(characters, e2);
            chance_max += chance;
            spawn_stat_level(world, e2, rsoul, 2);
            spawn_stat_state(world, e2, rhealth, 8, 8);
        }
        // add our skeleton prefab
        // Disabled for now
        /*{
            byte chance = 5;
            char* svox_name = "grazor";
            entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(svox_name));
            entity e2 = spawn_character3_meta(world, prefab_character3_meta,
 prefab_character3_skeleton_npc, svox_name, model, chance);
            add_to_CharacterLinks(characters, e2);
            chance_max += chance;
        }*/
        chance->value = chance_max;

        zox_logv("At [%f] Realm [characters] [%i] spawned.", zox_current_time, characters->length);
    }
} zox_sys_end(Character3RealmSpawnSystem);
