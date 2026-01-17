// All Game Menu Parent
entity spawn_prefab_menu_game(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("menu_game");
    zox_prefab_set(e, Children, { 0 });
    // stretchhes to canvas
    return e;
}