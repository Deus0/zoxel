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
    entity character = zox_getv(player, CharacterLink);
    if (!zox_valid(character)) {
        return;
    }
    zox_geter_value(player, GameLink, entity, game);
    zox_geter_value(game, RealmLink, entity, realm);
    if (!zox_valid(realm)) {
        return;
    }
    entity skillbook = zox_get_child_by_id(world, character, zox_id(Skillbook));
    zox_geter(realm, SkillLinks, rskills);
    zox_log("Giving [%s] [%i] Skills.", zox_get_name(character), rskills->length);
    for (int j = 0; j < rskills->length; j++) {
        entity rskill = rskills->value[j];
        if (!zox_valid(rskill)) {
            zox_log_error("Skill invalid [%i]", j)
            continue;
        }
        entity slot = zox_get_empty_slot(world, skillbook);
        if (!zox_valid(slot)) {
            zox_logw("[Skillbook] Out of empty slots.");
            zox_print_slots(world, skillbook);
            break;
        }
        entity skill = spawn_user_skill(world, character, rskill);
        zox_muter(slot, DataLink, slot_data);
        slot_data->value = skill;
        zox_muter(slot, DataDirty, dirty);
        dirty->value = zox_dirty_trigger;
        zox_log("   + [%s]", zox_get_name(rskill));
    }
    tst_all_skills = 1;
}
