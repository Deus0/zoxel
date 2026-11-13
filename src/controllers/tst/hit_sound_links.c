// tst: hit_sound_links, blocks and npcs link what they sound like hit (sounds/bop/hits.c, doc/sounds.md)
//  - every realm block kind links a sound entity with its effect, stone ones a knock, dirt a thud
//  - every npc links its kind's sound, slimes a squelch, the rest voiced flesh (they grunt)
//  - run: bash bsh/tst.sh hit_sound_links
byte zox_tst_hit_sound_links_ready(ecs* world) {
    return zox_tst_npc_stats_ready(world);
}

void zox_tst_hit_sound_links_check(ecs* world) {
    entity realm = 0;
    int npcs = 0, npcs_unlinked = 0, slimes_wrong = 0, flesh_wrong = 0;
    ecs_iter_t it = ecs_each_id(world, zox_id(Character));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const entity e = it.entities[i];
            if (!zox_tst_npc_ready(world, e)) {
                continue;
            }
            realm = zox_get_link(world, e, RealmLink);
            npcs++;
            const entity sound = zox_get_link(world, e, HitSoundLink);
            if (!zox_valid(sound) || !zox_has(sound, SoundEffectData)) {
                npcs_unlinked++;
                continue;
            }
            const byte instrument = zox_get(sound, SoundEffectData)->value.instrument;
            if (zox_has(e, CharacterSlime)) {
                slimes_wrong += instrument != instrument_squelch;
            } else if (instrument == instrument_impact) {
                flesh_wrong += !zox_has(sound, HitSoundVoiced);
            }
        }
    }
    zox_log("[tst] [metric] hit_sound_links [%i] npcs, [%i] unlinked", npcs, npcs_unlinked);
    zox_tst_check(npcs, "no npcs");
    zox_tst_check(!npcs_unlinked, "[%i] of [%i] npcs have no hit sound", npcs_unlinked, npcs);
    zox_tst_check(!slimes_wrong, "[%i] slimes don't squelch", slimes_wrong);
    zox_tst_check(!flesh_wrong, "[%i] flesh npcs don't grunt", flesh_wrong);
    zox_tst_check(zox_valid(realm), "no realm");
    if (!zox_valid(realm)) {
        return;
    }
    zox_geter(realm, BlockLinks, blocks);
    int unlinked = 0, stone_wrong = 0, dirt_wrong = 0;
    for (int i = 0; i < blocks->length; i++) {
        const entity block = blocks->value[i];
        if (!zox_valid(block)) {
            continue;
        }
        const entity sound = zox_get_link(world, block, HitSoundLink);
        if (!zox_valid(sound) || !zox_has(sound, SoundEffectData)) {
            zox_log("[tst] block [%s] has no hit sound", zox_get_name(block));
            unlinked++;
            continue;
        }
        const byte instrument = zox_get(sound, SoundEffectData)->value.instrument;
        stone_wrong += zox_has(block, BlockStone) && instrument != instrument_knock;
        dirt_wrong += zox_has(block, BlockSoil) && instrument != instrument_thud;
    }
    zox_log("[tst] [metric] hit_sound_links [%i] blocks, [%i] unlinked", blocks->length, unlinked);
    zox_tst_check(blocks->length, "the realm has no blocks");
    zox_tst_check(!unlinked, "[%i] of [%i] blocks have no hit sound", unlinked, blocks->length);
    zox_tst_check(!stone_wrong && !dirt_wrong, "stone [%i] or dirt [%i] blocks sound wrong", stone_wrong, dirt_wrong);
}
