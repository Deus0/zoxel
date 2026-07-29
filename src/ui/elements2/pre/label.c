entity spawn_prefab_label(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("label");
    zox_add_tag(e, Label);
    zox_add_tag(e, TextResizer);
    zox_add_tag(e, FixToLayout);
    return e;
}
