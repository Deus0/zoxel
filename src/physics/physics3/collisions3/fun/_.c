#include "systems.c"
#include "debug.c"
// a body's capsule collider: a child (radius, total height, centre offset from the body), linked by ColliderLink;
//  game code calls it for bodies on a poly terrain (src/terrains/poly/collisions)
entity spawn_capsule_collider(ecs* world, entity body, float radius, float height, float3 offset) {
    const entity e = zox_new();
    zox_add(e, Capsule);
    zox_setv(e, RadiusFloat, radius);
    zox_setv(e, HeightFloat, height);
    zox_setv(e, LocalPosition3D, offset);
    zox_set_parent_non_fragment(world, e, body);
    zox_set_link(world, body, ColliderLink, e);
    return e;
}
