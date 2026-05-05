entity spawn_prefab_text_input(ecs* world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("text_input");
    zox_add_tag(e, TextInput);
    return e;
}
