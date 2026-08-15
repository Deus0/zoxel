entity spawn_prefab_menu_play(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("menu_play");
    zox_add(e, MenuPlay);
    zox_prefab_set(e, AnchorSize, { float2_one });
    return e;
}