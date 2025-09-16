entity spawn_prefab_dialogue_ui(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("dialogue_ui");
    zox_add_tag(e, DialogueUI);
    zox_prefab_set(e, DialogueRunLink, { 0 });
    zox_prefab_set(e, TargetText, { 0 });
    return e;
}
