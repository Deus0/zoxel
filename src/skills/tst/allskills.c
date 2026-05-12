byte tst_all_skills = 0;

// TODO: Work Skill Slots better
void zox_tst_all_skills(ecs* world, ClickEventData data) {
    if (tst_all_skills) {
        zox_log("Already given player character all skills.");
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    if (!zox_valid(character) || !zox_valid(realm)) {
        return;
    }
    zox_geter(realm, SkillLinks, rskills);
    zox_muter(character, SkillLinks, skills);
    zox_log("Giving [%s] [%i] Skills.", zox_get_name(character), rskills->length);
    for (int j = 0; j < rskills->length; j++) {
        entity rskill = rskills->value[j];
        if (!zox_valid(rskill)) {
            zox_log_error("Skill invalid [%i]", j)
            continue;
        }
        entity skill = spawn_user_skill(world, character, rskill);
        add_to_SkillLinks(skills, skill);
        zox_log("   + [%s]", zox_get_name(rskill));
    }
    tst_all_skills = 1;
}