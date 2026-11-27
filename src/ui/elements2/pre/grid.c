entity spawn_prefab_grid(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("grid");
    // Grid
    zox_setv(e, GridPadding, byte2_zero);
    zox_setv(e, GridMargins, byte2_zero);
    // scrolled by rows in its scroll view (windows/ins/grid.c): its Mask clips the rows past its edges, ListStart its
    //  first shown row, ListVisible how many fit (every one till a window measures them)
    zox_add(e, Mask);
    zox_setv(e, ListStart, 0);
    zox_setv(e, ListVisible, 255);
    zox_add(e, Refresh);    // laid out once spawned (grid_system on Update); add Refresh again when its elements change
    return e;
}
