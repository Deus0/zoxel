entity spawn_prefab_label(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("label");

    zox_add_tag(e, Label);
    zox_add_tag(e, TextResizer);

    return e;
}
