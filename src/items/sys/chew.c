// NOTE: Eating chews: bites while the hand is at the mouth (the eat swing, swing.c), each a crunchy munch and crumbs
//  falling from the mouth in the food's colours (its model's palette)
//  - over the food's cooldown, when the eat swing holds the hand at the mouth: bites at eat_bites of it
//  - the eat swing's own sound (sound_effect_consume) plays at the use (consume.c), the munches are the chewing
#define eat_bites_count 3
const float eat_bites[eat_bites_count] = { 0, 0.3f, 0.6f };     // of the cooldown
#define eat_crumbs 6
#define eat_crumb_life 0.7f
const SoundEffect eat_munch = { "munch", instrument_crunch, 48, 53, 0.08f, 0.11f, 2.2f, 2.6f };
uint zox_eat_bites = 0;     // counted for tst eat_swing

// crumbs from the hand (at the mouth) thrown a little out ahead of the face and falling, the food's colours
static void eat_crumbs_spawn(ecs* world, entity food, float3 at, float3 face, uint seed) {
    const entity model = zox_get_link(world, food, ModelLink);
    const ColorRGBs* colors = zox_valid(model) && zox_has(model, ColorRGBs) ? zox_get(model, ColorRGBs) : NULL;
    for (int i = 0; i < eat_crumbs; i++) {
        color c = (color) { 190, 140, 80, 255 };    // a crumb, without a palette
        if (colors && colors->length) {
            const color_rgb rgb = colors->value[seed_rand(seed + i) % colors->length];
            c = (color) { rgb.r, rgb.g, rgb.b, 255 };
        }
        const entity e = spawn_particle3(world, float3_add(at, float3_scale(face, 0.05f)), float3_zero, c);
        const float3 spread = { seed_rangef(seed + i * 3, -0.3f, 0.3f), seed_rangef(seed + i * 3 + 1, 0.2f, 0.6f),
            seed_rangef(seed + i * 3 + 2, -0.3f, 0.3f) };
        zox_setv(e, Velocity3D, float3_add(float3_scale(face, seed_rangef(seed + i * 7, 0.5f, 0.9f)), spread));
        zox_setv(e, Gravity3D, ((float3) { 0, -6, 0 }));
        zox_setv(e, DestroyInTime, eat_crumb_life);
    }
}

void eat_chew_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const float delta = zox_delta_time;
    zox_sys_begin();
    zox_sys_in(CooldownAt);
    zox_sys_in(CooldownTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CooldownAt, at);
        zox_sys_i(CooldownTime, time);
        if (!at->value || time->value <= 0) {
            continue;
        }
        const float now = (float) ((zox_current_time - at->value) / time->value);
        const float last = now - delta / (float) time->value;
        const entity user = zox_get_parent(world, e);
        const entity skeleton = zox_valid(user) ? get_skeleton(world, user) : 0;
        const entity hand = zox_valid(skeleton) ? zox_get_link(world, skeleton, HandBoneLink) : 0;
        const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
        if (!zox_valid(hand) || !zox_valid(head) || zox_getv(skeleton, SwingKind) != zox_swing_eat) {
            continue;
        }
        for (int j = 0; j < eat_bites_count; j++) {
            if (last < eat_bites[j] && now >= eat_bites[j]) {
                const entity sound = spawn_sound_effect_data(world, &eat_munch);
                set_sound_source(world, sound, user);
                const float3 face = float4_rotate_float3(zox_getv(head, Rotation3D), float3_forward);    // the face is +z
                eat_crumbs_spawn(world, e, zox_getv(hand, Position3D), face, (uint) (zox_current_time * 1000) + j * 31);
                zox_eat_bites++;
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(eat_chew_system);
