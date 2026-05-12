zox_sys2(CharacterSkillsSpawnSystem) {
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
        // Give skill?
        // When Testing all skills
        // TODO: Remove this and make a test function
        if (!test_all_skills) {
            continue;
        }
        zox_geter(realm->value, SkillLinks, rskills);
        int place_index = 0;
        for (int j = 0; j < rskills->length; j++) {
            if (j >= skills_count) {
                break;
            }
            entity rskill = rskills->value[j];
            if (!zox_valid(rskill)) {
                zox_log_error("Skill invalid [%i]", j)
                continue;
            }
            skills->value[place_index++] = spawn_user_skill(world, e, rskill);
        }
    }
} zox_sys_end(CharacterSkillsSpawnSystem);
