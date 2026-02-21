extern entity prefab_character3_npc;
extern entity prefab_character3_instanced_npc;
extern entity prefab_character3_skeleton_npc;

zox_sys2(Character3RealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(ModelLinks);
    zox_sys_out(CharacterLinks);
    zox_sys_out(CharactersChanceMax);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(ModelLinks, models);
        zox_sys_o(CharacterLinks, characters);
        zox_sys_o(CharactersChanceMax, chance);

        if (state->value != zox_generate_realm_characters) {
            continue;
        }
/*void spawn_realm_characters(ecs *world, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    if (!zox_has(e, CharacterLinks)) {
        zox_log_error("realm does not have CharacterLinks [%s]", zox_get_name(e))
        return;
    }

    zox_geter(e, CharacterLinks, old);
    if (old) {
        for (int i = 0; i < old->length; i++) {
            const entity e2 = old->value[i];
            if (zox_valid(e2)) {
                zox_delete(e2)
            }
        }
    }*/

        byte chance_max = 0;
        // zox_geter(e, ModelLinks, models);
        // CharacterLinks characters = (CharacterLinks) { 0 };

        // add files
        int count = 5; // count of below array
        char* vox_names[] = { "slime", "chicken", "mrpenguin", "bob", "bigmrpenguin" };
        byte chances[] = { 30, 30, 8, 8, 3 };

        entity prefab_character = is_characters_instanced ? prefab_character3_instanced_npc : prefab_character3_npc;

        for (int i = 0; i < count; i++) {
            entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(vox_names[i]));
            if (!zox_valid(model)) {
                continue;
            }

            // can choose here properties for spawning
            byte chance = chances[i];
            const entity e2 = spawn_character3_meta(
                world,
                prefab_character3_meta,
                prefab_character,
                vox_names[i],
                model,
                chance
            );
            add_to_CharacterLinks(characters, e2);
            chance_max += chance;
        }

        // add model links with tag ModelCharacter
        for (int i = 0; i < models->length; i++) {
            entity model = models->value[i];

            if (!zox_valid(model)) {
                zox_log_error("realm has invalid model [%i]", i)
                continue;
            }

            if (!zox_has(model, ModelCharacter)) {
                continue;
            }

            byte chance = 8;
            entity e2 = spawn_character3_meta(
                world,
                prefab_character3_meta,
                prefab_character,
                "character",
                model,
                chance
            );
            add_to_CharacterLinks(characters, e2);

            chance_max += chance;
        }

        // add our skeleton prefab
        {
            byte chance = 5;
            char* svox_name = "grazor";
            entity model = string_hashmap_get(files_hashmap_voxes, new_string_data(svox_name));

            entity e2 = spawn_character3_meta(
                world,
                prefab_character3_meta,
                prefab_character3_skeleton_npc,
                svox_name,
                model,
                chance
            );
            add_to_CharacterLinks(characters, e2);
            chance_max += chance;
        }
        chance->value = chance_max;

        // zox_set(e, CharactersChanceMax, { chance_max });
        // zox_set_ptr(e, CharacterLinks, characters);

        zox_logv("At [%f] Realm [characters] [%i] spawned.", zox_current_time, characters->length);
    }
} zox_sys_end(Character3RealmSpawnSystem);
