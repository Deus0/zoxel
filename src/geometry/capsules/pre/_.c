entity prefab_capsule_mesh;

// a lit mesh with a capsule's shape, its mesh built from it (eve/_.c)
static void spawn_prefabs_capsules(ecs* world) {
    zox_prefab_child(prefab_lit_mesh);
    zox_prefab_name("capsule_mesh");
    zox_add(e, CapsuleMesh);
    zox_setv(e, RadiusFloat, 0.35f);
    zox_setv(e, HeightFloat, 1);
    zox_setv(e, ColorRGB, ((color_rgb) { 230, 120, 60 }));
    prefab_capsule_mesh = e;
}
