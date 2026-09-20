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
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        return;
    }
    entity game = zox_get_link(world, player, GameLink);
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm)) {
        return;
    }
    entity skillbook = zox_get_child_by_id(
        world,
        character,
        zox_id(Skillbook));
    zox_log("Giving [%s] [X] Skills.",
            zox_getn(character)); //,
            //rskills->length);
    iter it2 = zox_children(world, realm);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity rskill = it2.entities[j];
            if (!zox_has(rskill, Skill)) {
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
            zox_add(slot, DataDirty);
            zox_log("   + [%s]",
                zox_getn(rskill));
        }
    }
    tst_all_skills = 1;
}
