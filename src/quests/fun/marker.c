// quest markers over a quest giver's head, from the quest taker's view of its quest:
//  ! yellow - it has a quest to give, ? gray - doing it, ? yellow - can hand it in, none - handed in

// the realm quest a quest giver gives (0 when it gives none)
entity get_quest_giver_quest(
    ecs* world,
    entity quest_giver)
{
    entity giving = zox_get_child_by_id(world, quest_giver, zox_id(QuestGiving));
    return zox_valid(giving) ? zox_get_prefab(world, giving) : 0;
}

// a player character's copy of the quest a quest giver gives (0 when no one has it)
entity get_players_user_quest(
    ecs* world,
    entity quest_giver)
{
    const entity quest = get_quest_giver_quest(world, quest_giver);
    if (!zox_valid(quest)) {
        return 0;
    }
    for (byte i = 0; i < players_playing; i++) {
        const entity player = zox_players[i];
        const entity character = zox_valid(player) ? get_player_character(world, player) : 0;
        if (!zox_valid(character)) {
            continue;
        }
        const entity user_quest = get_user_quest(world, character, quest);
        if (user_quest) {
            return user_quest;
        }
    }
    return 0;
}

// the marker's text for a user quest (0: none, the quest is handed in)
static inline const char* get_quest_marker_text(
    ecs* world,
    entity user_quest)
{
    if (!user_quest) {
        return quest_marker_available;
    }
    if (zox_has(user_quest, QuestHandedin)) {
        return 0;
    }
    return zox_has(user_quest, QuestDone) ?
        quest_marker_done :
        quest_marker_doing;
}

static inline color get_quest_marker_fill(
    ecs* world,
    entity user_quest)
{
    if (user_quest && !zox_has(user_quest, QuestDone)) {
        return quest_marker_doing_fill;
    }
    return quest_marker_fill;
}

// sets a marker to show a user quest's state (user_quest 0: it has the quest to give)
void set_quest_marker(
    ecs* world,
    entity marker,
    entity user_quest)
{
    if (!zox_valid(marker)) {
        return;
    }
    const char* text = get_quest_marker_text(world, user_quest);
    if (!text) {
        return;
    }
    entity text_entity = zox_get_child_by_id_recursive(
        world,
        marker,
        zox_id(Text));
    if (!text_entity) {
        zox_loge("Marker text not found");
        return;
    }
    set_entity_text_fill(world, text_entity, get_quest_marker_fill(world, user_quest));
    set_entity_text(world, text_entity, text);
}

// links a user quest to the marker of the npc that gave it, so its progress updates the marker
void link_quest_marker(
    ecs* world,
    entity user_quest,
    entity marker)
{
    if (!zox_valid(user_quest) || !zox_valid(marker)) {
        return;
    }
    zox_link(world, user_quest, MarkerLink, marker);
    zox_link(world, marker, QuestLink, user_quest);
}
