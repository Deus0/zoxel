// from now not needed... just change settings in base camera to remove any frame issues
// later to swap cameras ill probably need to switch them over a frame or something fancier
// as i need to control when it updates exactly
entity prefab_free_camera;

entity spawn_prefab_free_camera(ecs *world, const entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("free_camera");
    zox_add(e, Perspective);
    prefab_free_camera = e;
    return e;
}
