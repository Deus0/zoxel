#include "particles_render.c"
#include "spawn_particles.c"

void zox_tsts_particles3(ecs* world) {
    zox_tst_add("particles_render", zox_tst_particles_render_ready, zox_tst_particles_render_check, 120);
    zox_tst_add("spawn_particles", zox_tst_spawn_particles_ready, zox_tst_spawn_particles_check, 120);
}
