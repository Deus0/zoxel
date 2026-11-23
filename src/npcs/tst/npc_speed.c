// tst: npc_speed, a slime is slow and slow witted at the start, a villager quicker (stats/fun/speed.c)
//  - a realm of its own (zox_tst_npc_world): its slime and villager kinds' speed and intelligence stats
//  - the slime under 0.7 of a player's speed and the villager's, it watches its threat longer between attacks
//    (npc_observe_time), and squashes longer before it pounces (pounce_crouch / its speed)
//  - run: bash bsh/tst.sh npc_speed
byte zox_tst_npc_speed_ready(ecs* world) {
    entity realm, terrain;
    return zox_tst_npc_world(world, &realm, &terrain);
}

void zox_tst_npc_speed_check(ecs* world) {
    const entity realm = zox_tst_world_realm;
    const entity slime = zox_tst_realm_slime(world, realm), villager = zox_tst_realm_humanoid(world, realm);
    zox_tst_check(zox_valid(slime) && zox_valid(villager), "the realm has no slime or villager");
    if (!zox_valid(slime) || !zox_valid(villager)) {
        return;
    }
    const float slime_speed = character_stat_tagged(world, slime, zox_id(StatSpeed), -1);
    const float villager_speed = character_stat_tagged(world, villager, zox_id(StatSpeed), -1);
    const float slime_wits = character_stat_tagged(world, slime, zox_id(StatIntelligence), -1);
    const float villager_wits = character_stat_tagged(world, villager, zox_id(StatIntelligence), -1);
    const float slime_watch = npc_observe_time(world, slime), villager_watch = npc_observe_time(world, villager);
    zox_log("[tst] npc_speed slime speed [%.2f] intelligence [%.1f] watches [%.2fs] squashes [%.2fs], villager [%.2f] [%.1f] [%.2fs]",
        slime_speed, slime_wits, slime_watch, pounce_crouch / character_speed(world, slime), villager_speed, villager_wits,
        villager_watch);
    zox_tst_check(slime_speed > 0 && slime_speed < 0.7f, "the slime's speed is [%.2f], not slow", slime_speed);
    zox_tst_check(villager_speed > slime_speed, "the villager [%.2f] isn't quicker than the slime [%.2f]", villager_speed, slime_speed);
    zox_tst_check(slime_wits >= 0 && villager_wits > slime_wits, "intelligence: slime [%.1f] villager [%.1f]", slime_wits, villager_wits);
    zox_tst_check(slime_watch > 0.9f && slime_watch > villager_watch, "the slime watches [%.2fs] between attacks, the villager [%.2fs]",
        slime_watch, villager_watch);
}

void zox_tsts_npc_speed(ecs* world) {
    zox_tst_add("npc_speed", zox_tst_npc_speed_ready, zox_tst_npc_speed_check, 120);
    zox_tst_set_setup("npc_speed", NULL, 1);
}
