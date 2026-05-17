entity spawn_prefab_quest(ecs* world) {
    zox_prefab();
    zox_prefab_name("quest");
    zox_add_tag(e, Quest);
    zox_prefab_set(e, ZoxName, { 0 });
    zox_prefab_set(e, TextureLink, { 0 });
    zox_prefab_set(e, QuestDirty, { 0 });
    // zox_prefab_set(e, Color, { color_white });
    return e;
}
