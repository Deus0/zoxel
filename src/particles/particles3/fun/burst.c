// A burst of particles in a vox's colours: each a random one of its palette (ColorRGBs), i.e. crumbs of a food eaten,
//  chips of a block broken; without a palette, the burst's colour
typedef struct {
    float3 position;
    float3 velocity;    // each's, plus a random spread
    float3 spread;      // the random velocity, each way
    float3 gravity;
    float life;         // seconds
    color fallback;     // without a vox palette
    byte count;
} particle3_burst;

void spawn_particle3_burst(ecs* world, entity vox, const particle3_burst* burst, uint seed) {
    const ColorRGBs* colors = zox_valid(vox) && zox_has(vox, ColorRGBs) ? zox_get(vox, ColorRGBs) : NULL;
    for (int i = 0; i < burst->count; i++, seed += 4) {
        color c = burst->fallback;
        if (colors && colors->length) {
            const color_rgb rgb = colors->value[seed_rand(seed) % colors->length];
            c = (color) { rgb.r, rgb.g, rgb.b, 255 };
        }
        const entity e = spawn_particle3(world, burst->position, float3_zero, c);
        const float3 spread = burst->spread;
        zox_setv(e, Velocity3D, float3_add(burst->velocity, ((float3) { seed_rangef(seed + 1, -spread.x, spread.x),
            seed_rangef(seed + 2, -spread.y, spread.y), seed_rangef(seed + 3, -spread.z, spread.z) })));
        zox_setv(e, Gravity3D, burst->gravity);
        zox_setv(e, DestroyInTime, burst->life);
    }
}
