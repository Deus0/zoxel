void CharacterSkillsSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RealmLink);
    zox_sys_out(SkillLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(SkillLinks, skills);

        if (state->value != zox_dirty_active) {
            continue;
        }

        int skills_count = 8;
        initialize_SkillLinks(skills, skills_count);
        if (!skills->value) {
            zox_log_error("Failed allocating memory for skills");
            continue;
        }

        for (int j = 0; j < skills_count; j++) {
            skills->value[j] = 0; // blanks are item slots
        }

        if (!test_all_skills) {
            continue;
        }

        zox_geter(realm->value, SkillLinks, realm_skills);

        int place_index = 0;
        for (int j = 0; j < realm_skills->length; j++) {
            if (j >= skills_count) {
                break;
            }

            const entity skill = realm_skills->value[j];
            if (!zox_valid(skill)) {
                zox_log_error("skill invalid [%i]", j)
                continue;
            }

            skills->value[place_index++] = spawn_user_skill(world, skill, e);
        }

    }
} zoxd_system2(CharacterSkillsSystem);