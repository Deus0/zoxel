// A melee swing's hit mark (SwingMark on a punch swing, bones/sys/swing_mark.c): its skill hits what its user aims at
//  now (melee_hit), so the damage lands as the fist does, the windup only animation
void on_melee_hit(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SwingMark);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingMark, mark);
        if (mark->value != 1 || zox_getv(e, SwingKind) != zox_swing_punch) {
            continue;
        }
        const entity skill = zox_getv(e, SwingAction);
        if (zox_valid(skill) && zox_has(skill, Melee)) {
            melee_hit(world, skill, zox_get_parent(world, e));
        }
    }
}

void zox_events_skills(ecs* world) {
    zox_observer(on_melee_hit, EcsOnSet, [in] bones.SwingMark, [none] bones.Skeleton);
}
