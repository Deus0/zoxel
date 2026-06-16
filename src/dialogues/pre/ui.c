/*entity spawn_prefab_dialogue_ui(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("dialogue_ui");
    zox_add_tag(e, DialogueUI);
    zox_add_tag(e, NavigationWindow);
    zox_prefab_set(e, DialogueProcessLink, { 0 });
    zox_prefab_set(e, DialogueTextLink, { 0 });
    zox_prefab_set(e, TargetText, { 0 });
    zox_prefab_set(e, AnimateTextBegin, { 0 });
    zox_prefab_set(e, AnimateTextTime, { 0.1f });
    zox_prefab_set(e, AnimateTextTimeLimits, { { 0.1f, 0.2f } });
    zox_prefab_set(e, ZigelSpawnedDirty, { 0 });
    return e;
}*/

entity spawn_prefab_dialogue_text(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("dialogue_text");
    zox_add_tag(e, DialogueLabel);
    zox_prefab_set(e, TargetText, { 0 });
    zox_prefab_set(e, AnimateTextBegin, { 0 });
    zox_prefab_set(e, AnimateTextTime, { 0.1f });
    zox_prefab_set(e, AnimateTextTimeLimits, { { 0.1f, 0.2f } });
    zox_prefab_set(e, ZigelSpawnedDirty, { 0 });
    zox_prefab_set(e, AnimateTextEnded, { 0 });
    return e;
}
