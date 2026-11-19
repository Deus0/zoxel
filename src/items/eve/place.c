// A place swing's hit mark (SwingMark, bones/sys/swing_mark.c): its block item places where its user aims now
//  (item_place), so the block goes in as the hand presses it
void on_place_hit(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SwingMark);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingMark, mark);
        if (mark->value != 1 || zox_getv(e, SwingKind) != zox_swing_place) {
            continue;
        }
        const entity item = zox_getv(e, SwingAction);
        if (zox_valid(item) && zox_has(item, ItemBlock)) {
            item_place(world, item, zox_get_parent(world, e));
        }
    }
}
