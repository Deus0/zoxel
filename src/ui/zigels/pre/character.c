entity spawn_zigel_prefab(ecs *world, entity prefab) {

    zox_prefab_child(prefab);
    zox_prefab_name("zigel");

    // zox_set(e, CanvasLink, { 1 });
    // Layouts2
    // zox_add_tag(e, FixToLayout);
    // zigel
    zox_add_tag(e, Zigel);
    zox_prefab_set(e, ZigelIndex, { 0 });

    // font texture
    zox_add_tag(e, FontTexture);
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateTexture, { zox_dirty_trigger });
    zox_prefab_set(e, Color, { color_white });
    zox_prefab_set(e, SecondaryColor, { color_white });
    zox_prefab_set(e, FontThickness, { 1 });
    zox_prefab_set(e, FontOutlineThickness, { 2 });

    return e;
}
