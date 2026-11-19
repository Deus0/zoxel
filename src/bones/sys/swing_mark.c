// A swing's marks: as a swing passes one, SwingMark is set to it (1 its first), so observers react to the animation,
//  i.e. eating's bites (actions/eve/eat.c) munch and drop crumbs as the hand's at the mouth
//  - the marks through a swing (0 to 1) by its kind; the hit is halfway (the warmup's end), its poses ik/sys/punch.c
typedef struct {
    byte count;
    float at[4];
} swing_marks;

const swing_marks swing_marks_of[] = {
    [zox_swing_punch] = { 1, { 0.5f } },               // the hit
    [zox_swing_place] = { 1, { 0.5f } },               // the block set
    [zox_swing_eat] = { 3, { 0.5f, 0.65f, 0.8f } },    // bites, the hand at the mouth
};
#define swing_kinds_count (sizeof(swing_marks_of) / sizeof(swing_marks))

void swing_mark_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SwingStart);
    zox_sys_in(SwingSpeed);
    zox_sys_in(SwingKind);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SwingStart, start);
        zox_sys_i(SwingSpeed, speed);
        zox_sys_i(SwingKind, kind);
        if (start->value <= 0 || speed->value <= 0 || kind->value >= swing_kinds_count) {
            continue;
        }
        const float now = (float) ((zox_current_time - start->value) / speed->value);
        const float last = now - zox_delta_time / speed->value;
        if (now < 0 || last > 1) {
            continue;
        }
        const swing_marks* marks = &swing_marks_of[kind->value];
        for (byte j = 0; j < marks->count; j++) {
            if (last < marks->at[j] && now >= marks->at[j]) {
                zox_set(e, SwingMark, { j + 1 });
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(swing_mark_system);
