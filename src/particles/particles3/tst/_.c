static byte zox_tst_particle_prefabs_ready(ecs* world) {
    return zox_valid(prefab_particle3D) && zox_valid(prefab_particle3D_emitter);
}

#include "particles_render.c"
#include "spawn_particles.c"
#include "burst.c"
#include "emit.c"
#include "stream.c"
#include "lifetime.c"

void zox_tsts_particles3(ecs* world) {
    zox_tst_add("particle_burst", zox_tst_particle_prefabs_ready, zox_tst_particle_burst_check, 30);
    zox_tst_set_setup("particle_burst", NULL, 1);
    zox_tst_add("particle_emit", zox_tst_particle_prefabs_ready, zox_tst_particle_emit_check, 30);
    zox_tst_set_setup("particle_emit", NULL, 1);
    zox_tst_add("particle_stream", zox_tst_particle_prefabs_ready, zox_tst_particle_stream_check, 30);
    zox_tst_set_setup("particle_stream", NULL, 1);
    zox_tst_add("particle_lifetime", zox_tst_particle_lifetime_ready, zox_tst_particle_lifetime_check, 30);
    zox_tst_set_setup("particle_lifetime", NULL, 1);
    zox_tst_add("particles_render", zox_tst_particles_render_ready, zox_tst_particles_render_check, 120);
    zox_tst_add("spawn_particles", zox_tst_spawn_particles_ready, zox_tst_spawn_particles_check, 120);
}
