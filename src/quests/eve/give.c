void zox_event_give_quest(iter* it) {
    byte dbg_log = 1;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // current node
        entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!node || !zox_has(node, NodeGiveQuest)) {
            continue;
        }
        // increment node tree
        zox_setv(node, NodeEnd, zox_dirty_trigger);
        // get speaker of process - FirstSpeaker
        entity quest = zox_get_link(world, node, QuestLink);
        if (!quest) {
            zox_loge("[zox_event_give_quest] Invalid [quest]");
            return;
        }
        entity quest_taker = get_speaker_a(world, e);
        entity quest_giver = get_speaker_b(world, e);
        if (!zox_valid(quest_taker) ||
            !zox_valid(quest_giver)
        ) {
            zox_loge("[zox_node_give_quest] Invalid [speaker]");
            return;
        }
        // TODO: Make sure we dont a already have it
        // NOTE: Give quest to quest taker
        spawn_user_quest(
            world,
            quest_taker,
            quest);
        // Spawn particles
        float3 bounds = zox_getv(quest_taker, Bounds3D);
        entity particles = spawn_particle3D_emitter(
            world,
            quest_taker,
            8,
            float3_scale(bounds, 3),
            (color) { 155, 155, 155, 88 });
        zox_setv(particles, DestroyInTime, 3);
        // TODO: Link Quest Givers quest to Quest Taker
        // TODO: Mark Quest Givers quest as Active
        // NOTE: Swap Marker of quest giver
        entity marker = zox_get_link(world, quest_giver, Marker);
        if (zox_valid(marker)) {
            // set to ?
            entity text = zox_get_child_by_id_recursive(
                world,
                marker,
                zox_id(Text));
            if (text) {
                set_entity_text(
                    world,
                    text,
                    "+");
            } else {
                zox_loge("Marker text not found");
            }
        } else {
            zox_loge("Marker not found");
        }
        if (dbg_log) {
            zox_log("[zox_node_give_quest] [%s] Given to [%s] From [%s]",
                zox_getn(quest),
                zox_getn(quest_taker),
                zox_getn(quest_giver));
        }
    }
}
