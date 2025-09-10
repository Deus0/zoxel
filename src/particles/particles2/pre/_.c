#include "particle.c"
entity prefab_particle2;

void spawn_prefabs_particles2D(ecs* world) {
    prefab_particle2 = spawn_prefab_particle2(world);
}