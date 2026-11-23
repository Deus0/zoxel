// tst: body_drops, a dying character drops one of its body parts rarely, two more rarely, never its whole body
//  - a villager in a realm of its own (zox_tst_npc_world): its body parts are its Item children tagged BodyPart, which
//    character_item_drop_system holds back (it dropped them all)
//  - body_parts_dropped over every roll: none most of the time, at most 2
//  - run: bash bsh/tst.sh body_drops
byte zox_tst_body_drops_stage = 0;
int zox_tst_body_drops_frames = 0;
entity zox_tst_body_drops_npc = 0;

byte zox_tst_body_drops_ready(ecs* world) {
    entity realm, terrain;
    zox_tst_body_drops_frames++;
    zox_tst_progress(zox_tst_body_drops_stage * 1000 + zox_tst_body_drops_frames);
    if (!zox_tst_npc_world(world, &realm, &terrain)) {
        return 0;
    }
    if (zox_tst_body_drops_stage == 0) {
        zox_tst_body_drops_npc = spawn_character3_npc(world, zox_tst_realm_humanoid(world, realm), realm, terrain, 1, 0, 0,
            zox_tst_npc_void, quaternion_identity, NULL);
        zox_tst_body_drops_stage = 1;
        zox_tst_body_drops_frames = 0;
        return 0;
    }
    zox_tst_waiting = "its body";
    return zox_tst_skeletons_built(world, zox_tst_body_drops_npc) || zox_tst_body_drops_frames > 600;
}

void zox_tst_body_drops_check(ecs* world) {
    const entity e = zox_tst_body_drops_npc;
    zox_tst_check(zox_valid(e), "the villager is gone");
    if (!zox_valid(e)) {
        return;
    }
    entity items[zox_children_capacity];
    const uint count = zox_get_children_by_id(world, e, items, zox_children_capacity, zox_id(Item));
    uint parts = 0;
    for (uint i = 0; i < count; i++) {
        parts += zox_valid(items[i]) && zox_has(items[i], BodyPart);
    }
    int rolled[3] = { 0, 0, 0 };
    for (int roll = 0; roll < 100; roll++) {
        const byte n = body_parts_dropped(roll);
        rolled[n < 3 ? n : 2] += n < 3;
    }
    zox_log("[tst] body_drops items [%i] body parts [%i], rolls of 100: none [%i] one [%i] two [%i]", count, parts,
        rolled[0], rolled[1], rolled[2]);
    zox_tst_check(parts >= 4, "the villager has [%i] body part items, the drops can't hold them back", parts);
    zox_tst_check(rolled[0] + rolled[1] + rolled[2] == 100, "a roll dropped more than 2 parts");
    zox_tst_check(rolled[0] >= 85, "only [%i] in 100 deaths drop no part, it should be rare", rolled[0]);
    zox_tst_check(rolled[1] > 0 && rolled[2] > 0 && rolled[2] < rolled[1], "one part [%i] or two [%i] never drop, or two drop more",
        rolled[1], rolled[2]);
}

void zox_tsts_body_drops(ecs* world) {
    zox_tst_add("body_drops", zox_tst_body_drops_ready, zox_tst_body_drops_check, 120);
    zox_tst_set_setup("body_drops", NULL, 1);
}
