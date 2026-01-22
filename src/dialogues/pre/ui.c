entity spawn_prefab_dialogue_ui(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("dialogue_ui");
    zox_add_tag(e, DialogueUI);
    zox_prefab_set(e, DialogueProcessLink, { 0 });
    zox_prefab_set(e, TargetText, { 0 });
    zox_add_tag(e, NavigationWindow);
    return e;
}
