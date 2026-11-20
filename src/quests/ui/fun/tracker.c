// an entity's name for the tracker: its ZoxName, else the prefab's it's from (a quest just given has its realm
//  quest's before its own), else its entity name, else ??? (an unnamed one printed (null))
static inline const char* get_quest_tracker_name(
    ecs* world,
    entity e)
{
    if (!zox_valid(e)) {
        return "???";
    }
    if (zox_has(e, ZoxName) && zox_get(e, ZoxName)->value[0]) {
        return zox_get(e, ZoxName)->value;
    }
    const entity from = zox_get_prefab(world, e);
    if (zox_valid(from) && zox_has(from, ZoxName) && zox_get(from, ZoxName)->value[0]) {
        return zox_get(from, ZoxName)->value;
    }
    const char* name = zox_getn(e);
    return name && name[0] ? name : "???";
}

// the tracker's text for a quest, its name and objective: always allocated, the caller frees it (an invalid quest's
//  was a literal "", freed when its tracker spawned)
char* get_quest_tracker_text(
    ecs* world,
    entity quest)
{
    char* text = malloc(512);
    if (!text) {
        return NULL;
    }
    text[0] = 0;
    if (!zox_valid(quest)) {
        return text;
    }
    int index = snprintf(text, 512, "%s", get_quest_tracker_name(world, quest));
    if (index < 0 || index >= 512) {
        return text;
    }
    if (zox_has(quest, SlayQuest)) {
        entity character = zox_get_link(world, quest, CharacterLink);
        // a quest just given has its realm quest (IsA, its name) before its own link: that one's (it showed ???)
        if (!zox_valid(character)) {
            const entity from = zox_get_prefab(world, quest);
            character = zox_valid(from) ? zox_get_link(world, from, CharacterLink) : 0;
        }
        const byte value = zox_has(quest, QuestValue) ? zox_getv(quest, QuestValue) : 0;
        const byte target = zox_has(quest, QuestTarget) ? zox_getv(quest, QuestTarget) : 0;
        snprintf(text + index, 512 - index, "\n- Slay [%s] [%i/%i]",
            get_quest_tracker_name(world, character), value, target);
    } else if (zox_has(quest, FindQuest)) {
        snprintf(text + index, 512 - index, "\n- Talk to a villager in a town");
    }
    return text;
}
