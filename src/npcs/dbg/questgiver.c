entity zox_tst_quest_giver;

void zox_tst_spawn_quest_giver_npc(
    ecs *world,
    ClickEventData data)
{
    byte dbg_inspector = 0;
    uint capacity = 256;
    if (zox_valid(zox_tst_quest_giver)) {
        zox_log("Cleaning Test [zox_tst_quest_giver]");
        zox_delete(zox_tst_quest_giver);
        zox_tst_quest_giver = 0;
        return;
    }
    entity player = dbg_player;
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        zox_logw("No Player Character.");
        return;
    }
    float3 sposition = zox_getv(character, Position3D);
    float4 srotation = zox_getv(character, Rotation3D);
    entity terrain = zox_getv(character, TerrainLink);
    if (!zox_valid(terrain)) {
        return;
    }
    entity realm = zox_getv(terrain, RealmLink);
    if (!zox_valid(realm)) {
        return;
    }
    zox_geter(realm, CharacterLinks, characters);
    lint seed = rand_range(0, 10000);
    uint mindex = rand_range(0, characters->length - 1);
    entity meta = characters->value[mindex];
    // entity model = zox_getv(meta, ModelLink);
    zox_log("Running Test [zox_tst_quest_giver]");
    zox_log("   - Meta [%s:%i]:[%lu]",
        zox_getn(meta),
        mindex,
        seed);
    entity e = spawn_character3(
        world,
        prefab_character3_skeleton_npc,
        realm,
        terrain,
        seed,
        0,
        0,
        sposition,
        srotation,
        "Quester");
    zox_setv(e, DefaultBehaviour, zox_behaviour_idle);
    // Quest Dialogue
    entity dialogues[capacity];
    uint length = zox_get_children_by_id(
        world,
        realm,
        dialogues,
        capacity,
        zox_id(QuestDialogue));
    if (!length) {
        zox_loge("No dialogue found on realm [%s]",
            zox_getn(realm));
        return;
    }
    entity dialogue = dialogues[rand() % length];
    zox_link(world, e, Dialogue, dialogue);
    // spawn quest
    entity quests[capacity];
    uint quests_length = zox_get_children_by_id(
        world,
        realm,
        quests,
        capacity,
        zox_id(Quest));
    // Add our test quest to our test character
    entity player_start_quest = quests[rand() % quests_length];
    spawn_user_quest(
        world,
        e,
        player_start_quest);
    // We need to link to the quest giver quest
    zox_tst_quest_giver = e;
    zox_log("   + Quest To Give [%s]",
        zox_getn(player_start_quest));
    if (dbg_inspector) {
        entity canvas = zox_get_link(world, player, Canvas);
        spawn_inspector(world, canvas, player, e);
    }
}
