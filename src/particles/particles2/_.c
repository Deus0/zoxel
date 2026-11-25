zox_tag(Particle2D);
#include "pre/_.c"
#include "sys/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_particles2(ecs* world) {
    zox_module(particles2);
    zoxd_tag(Particle2D);
    add_hook_spawn_prefabs(spawn_prefabs_particles2D);
#ifdef zox_tests
    zox_tsts_particles2();
#endif
}
