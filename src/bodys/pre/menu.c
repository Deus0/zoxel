entity spawn_prefab_menu_body(ecs *world, entity p) {

    zox_prefab_child(p);
    zox_prefab_name("menu_body");

    zox_add_tag(e, MenuBody);

    return e;
}
