// A character's castbar: above its actionbar (a child of its window), hidden until it casts (AutoHide, castbar_system
//  pokes it while casting), its cast's name on it
entity spawn_castbar(ecs* world, entity parent, entity character, int frame_size, byte font_size) {
    const int2 size = (int2) { frame_size * 5, frame_size / 2 };
    const entity2 e2 = spawn_ui_bar(world, prefab_bar2, parent, (int2) { 0, frame_size + size.y }, size, (float2) { 0.5f, 0 },
        font_size, (byte2) { 8, 4 }, castbar_back, castbar_front);
    const entity e = e2.x;
    zox_set_unique_name(e, "castbar");
    zox_add(e, Castbar);
    zox_setv(e, CastAction, 0);
    zox_setv(e, BarLevel, 0);
    zox_link(world, e, CharacterLink, character);
    zox_add(e2.y, CastbarText);
    zox_setv(e, AutoHide, castbar_hide_time);
    zox_setv(e, AutoHideTime, 0);
    return e;
}
