// Eating's bites (SwingMark on an eat swing, bones/sys/swing_mark.c): a crunchy munch and crumbs in the food's colours
//  (its model's palette) thrown out ahead of the face, the hand at the mouth
//  - the food is what's swung (SwingAction); the eat's own sound plays at its use (items/sys/consume.c)
const SoundEffect eat_munch = { "munch", instrument_crunch, 48, 53, 0.08f, 0.11f, 2.2f, 2.6f };
uint zox_eat_bites = 0;     // counted for tst eat_swing

void on_eat_bite(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SwingMark);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingMark, mark);
        if (!mark->value || zox_getv(e, SwingKind) != zox_swing_eat) {
            continue;
        }
        const entity user = zox_get_parent(world, e);
        const entity hand = zox_get_link(world, e, HandBoneLink);
        const entity head = zox_get_link(world, e, HeadBoneLink);
        if (!zox_valid(user) || !zox_valid(hand) || !zox_valid(head)) {
            continue;
        }
        set_sound_source(world, spawn_sound_effect_data(world, &eat_munch), user);
        const entity food = zox_getv(e, SwingAction);
        const float3 face = float4_rotate_float3(zox_getv(head, Rotation3D), float3_forward);   // the face is +z
        const particle3_burst crumbs = {
            .position = float3_add(zox_getv(hand, Position3D), float3_scale(face, 0.05f)),
            .velocity = float3_add(float3_scale(face, 0.7f), (float3) { 0, 0.4f, 0 }),
            .spread = { 0.3f, 0.2f, 0.3f },
            .gravity = { 0, -6, 0 },
            .life = 0.7f,
            .fallback = { 190, 140, 80, 255 },
            .count = 6 };
        spawn_particle3_burst(world, zox_valid(food) ? zox_get_link(world, food, ModelLink) : 0, &crumbs,
            (uint) (zox_current_time * 1000) + mark->value * 31);
        zox_eat_bites++;
    }
}

void zox_events_actions(ecs* world) {
    zox_observer(on_eat_bite, EcsOnSet, [in] bones.SwingMark, [none] bones.Skeleton);
}
