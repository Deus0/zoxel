// tst: aura_particles, an aura's particles follow its user, turn off with it, and don't pile up
//  (they only spawned in one spot: each aura turned on spawned another emitter, turning it off removed none)
//  - a character (characters/tst/character.c, a realm of its own, no game) held in the void, an aura skill on it
//    (prefab_skill_aura), turned on (Activate triggered, as its key does, skill_toggle_system)
//  - on: one emitter on the character, linked from the aura (ParticlesEmitter); moved along x, it stays on it
//  - off: no emitters left on it; on again: one, not two
//  - run: bash bsh/tst.sh aura_particles
byte zox_tst_aura_stage = 0;
int zox_tst_aura_frames = 0;
entity zox_tst_aura_skill = 0;
float3 zox_tst_aura_start;
int zox_tst_aura_on = -1, zox_tst_aura_off = -1, zox_tst_aura_again = -1;
byte zox_tst_aura_linked = 0;
float zox_tst_aura_lag = -1;

static inline int zox_tst_aura_emitters(ecs* world, entity user) {
    entity children[zox_children_capacity];
    const uint count = zox_get_children(world, user, children, zox_children_capacity);
    int emitters = 0;
    for (uint i = 0; i < count; i++) {
        emitters += zox_has(children[i], Particle3DEmitter);
    }
    return emitters;
}

byte zox_tst_aura_particles_ready(ecs* world) {
    const entity e = zox_tst_body_ready(world, 0, (float3) { 0, 400, 0 });
    if (!e) {
        return 0;
    }
    zox_tst_waiting = "the aura turned on, off and on";
    zox_tst_progress(zox_tst_aura_stage * 1000 + zox_tst_aura_frames);
    // held in the void (nothing to stand on), moving along x a tenth of a block a frame
    if (!zox_tst_aura_stage) {
        zox_tst_aura_start = zox_getv(e, Position3D);
    }
    const float3 last = zox_getv(e, Position3D);
    zox_setv(e, Position3D, float3_add(zox_tst_aura_start, (float3) { zox_tst_aura_frames * 0.1f, 0, 0 }));
    zox_setv(e, Velocity3D, float3_zero);
    zox_tst_aura_frames++;
    if (zox_tst_aura_stage == 0) {
        zox_tst_aura_skill = spawn_user_skill(world, e, prefab_skill_aura);
        zox_setv(zox_tst_aura_skill, SkillRange, 1);
        zox_setv(zox_tst_aura_skill, Color, ((color) { 255, 100, 0, 255 }));
        zox_setv(zox_tst_aura_skill, Activate, zox_dirty_trigger);
        zox_tst_aura_stage = 1;
        zox_tst_aura_frames = 0;
        return 0;
    }
    if (zox_tst_aura_frames < 30) {
        return 0;
    }
    const entity skill = zox_tst_aura_skill;
    if (zox_tst_aura_stage == 1) {
        zox_tst_aura_on = zox_tst_aura_emitters(world, e);
        const entity emitter = zox_get_link(world, skill, ParticlesEmitter);
        zox_tst_aura_linked = zox_valid(emitter);
        // a frame behind: the hierarchy places it after this sets the character
        zox_tst_aura_lag = zox_tst_aura_linked ? float3_distance(zox_getv(emitter, Position3D), last) : -1;
        zox_setv(skill, Activate, zox_dirty_trigger);
        zox_tst_aura_stage = 2;
        zox_tst_aura_frames = 0;
        return 0;
    }
    if (zox_tst_aura_stage == 2) {
        zox_tst_aura_off = zox_tst_aura_emitters(world, e);
        zox_setv(skill, Activate, zox_dirty_trigger);
        zox_tst_aura_stage = 3;
        zox_tst_aura_frames = 0;
        return 0;
    }
    zox_tst_aura_again = zox_tst_aura_emitters(world, e);
    return 1;
}

void zox_tst_aura_particles_check(ecs* world) {
    zox_log("[tst] [metric] aura_particles emitters on [%i] off [%i] on again [%i], linked [%i], [%.2f] behind its user",
        zox_tst_aura_on, zox_tst_aura_off, zox_tst_aura_again, zox_tst_aura_linked, zox_tst_aura_lag);
    zox_tst_check(zox_tst_aura_on == 1 && zox_tst_aura_linked, "on: [%i] emitters on the user, linked [%i], not one", zox_tst_aura_on, zox_tst_aura_linked);
    zox_tst_check(zox_tst_aura_lag >= 0 && zox_tst_aura_lag < 0.01f, "the emitter is [%.2f] off its user", zox_tst_aura_lag);
    zox_tst_check(zox_tst_aura_off == 0, "off: [%i] emitters left on the user", zox_tst_aura_off);
    zox_tst_check(zox_tst_aura_again == 1, "on again: [%i] emitters, not one", zox_tst_aura_again);
}

void zox_tsts_aura_particles(ecs* world) {
    zox_tst_add("aura_particles", zox_tst_aura_particles_ready, zox_tst_aura_particles_check, 60);
    zox_tst_set_setup("aura_particles", NULL, 1);
}
