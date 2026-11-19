// Swings: an arm's motions for actions (a punch, a place, eating), one entry each: its poses, its timing and its marks
//  - a swing goes from the guard back, out to land at its hit, holds, then recovers to the guard; through it 0 to 1,
//    over the action's swing time (SwingSpeed: its attack speed)
//  - its marks are its events (SwingMark, sys/swing_mark.c): the first is its hit, observers react (a punch's damage,
//    a place's block, eating's bites); the hand lands at the first
//  - a new skill's animation is a new kind here (and its observers in its module)
//  - played by ik/sys/swing.c (the arm's target, the body's twist)
#define zox_swing_punch 0
#define zox_swing_place 1
#define zox_swing_eat 2
#define zox_swing_kinds 3
#define zox_swing_marks_max 4

// the hand's place, of the arm's reach: out along the look, in to the body's middle, up; and the body's turn (radians,
//  its arm's side forward)
typedef struct {
    float out;
    float in;
    float up;
    float twist;
} swing_pose;

typedef struct {
    swing_pose back;    // wound up
    swing_pose out;     // landed
    byte has_off;       // the off hand (the left) moves too: to off_back as this winds up, off_out as it lands
    swing_pose off_back;
    swing_pose off_out;
    float back_end;     // through the swing: gone back, going out
    float held_end;     // held out to, then recovering
    byte strike_ease;   // going out (zox_ease_*): out snaps a punch, in_out eases a place
    byte marks_count;
    float marks[zox_swing_marks_max];   // through the swing, its first the hit
} swing_data;

// raised (an item or skill held): a fighting guard, the fist ahead of the chin and the elbow bent down (at 0.95 of the
//  reach the arm was held up straight, a salute from outside, tst punch_views), still in the first person view
//  - 0.8 of the reach, the elbow ~75 degrees
const swing_pose swing_guard = { 0.7f, 0.15f, 0.35f, 0 };

const swing_data swings[zox_swing_kinds] = {
    [zox_swing_punch] = {
        .back = { 0.45f, -0.04f, 0.05f, -0.35f },   // the fist pulled back low by the ribs, the body turned away
        .out = { 0.99f, 0.18f, 0, 0.3f },           // the arm straight, in to the middle, the body turned into it
        .back_end = 0.38f, .held_end = 0.62f, .strike_ease = zox_ease_out,
        .marks_count = 1, .marks = { 0.5f },        // the hit
        .has_off = 1,
        .off_back = { 0.8f, 0.12f, 0.36f, 0 },      // the left reaching out ahead as the right winds up
        .off_out = { 0.48f, 0.24f, 0.42f, 0 } },    // and pulled back to the chin as it lands (the recoil)
    [zox_swing_place] = {
        .back = { 0.55f, 0.02f, 0.55f, -0.08f },    // the block lifted up by the head
        .out = { 0.95f, 0.06f, 0.05f, 0.15f },      // pressed out and down where the head looks (the shoulder's below it)
        .back_end = 0.3f, .held_end = 0.66f, .strike_ease = zox_ease_in_out,
        .marks_count = 1, .marks = { 0.5f } },      // the block set
    [zox_swing_eat] = {
        .back = { 0.7f, 0.2f, -0.05f, 0 },          // drawn in low by the chest
        .out = { 0.5f, 0.45f, 0.3f, -0.05f },       // at the mouth, under the eyes, in to the middle
        .back_end = 0.2f, .held_end = 0.82f, .strike_ease = zox_ease_in_out,
        .marks_count = 3, .marks = { 0.5f, 0.65f, 0.8f } },   // bites, the first eats it
};

static inline swing_pose swing_pose_lerp(swing_pose a, swing_pose b, float t) {
    return (swing_pose) { a.out + (b.out - a.out) * t, a.in + (b.in - a.in) * t, a.up + (b.up - a.up) * t,
        a.twist + (b.twist - a.twist) * t };
}

// the off hand's swing: its own back and out, through the same timing
static inline swing_data swing_off(const swing_data* swing) {
    swing_data off = *swing;
    off.back = swing->off_back;
    off.out = swing->off_out;
    return off;
}

// a swing's pose, p through it: back, out to its hit, held, recovered to the guard
static inline swing_pose swing_pose_at(const swing_data* swing, float p) {
    const float hit = swing->marks[0];
    if (p < swing->back_end) {
        return swing_pose_lerp(swing_guard, swing->back, zox_ease_between(zox_ease_in_out, 0, swing->back_end, p));
    }
    if (p < hit) {
        return swing_pose_lerp(swing->back, swing->out, zox_ease_between(swing->strike_ease, swing->back_end, hit, p));
    }
    if (p < swing->held_end) {
        return swing->out;
    }
    return swing_pose_lerp(swing->out, swing_guard, zox_ease_between(zox_ease_in_out, swing->held_end, 1, p));
}
