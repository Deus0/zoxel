static const float zox_tst_capsule_radius = 0.4f;
static const float zox_tst_capsule_height = 1.8f;
static zox_tst_clock zox_tst_capsule_clock;
static entity zox_tst_capsule_body;
static entity zox_tst_capsule_steep;
static entity zox_tst_capsule_island;

static entity zox_tst_capsule_terrain(ecs* world, int2 cells, float2 size, const float* samples) {
    const entity terrain = zox_new();
    PolyHeights heights = { 0 };
    spinlock_init(&heights.lock);
    initialize_PolyHeights(&heights, (cells.x + 1) * (cells.y + 1));
    if (samples) {
        memcpy(heights.value, samples, heights.length * sizeof(float));
    } else {
        poly_height_generate(heights.value, cells, size, 666);
    }
    zox_add(terrain, PolyTerrain);
    zox_setv(terrain, PolyCells, cells);
    zox_setv(terrain, Size2, size);
    zox_setv(terrain, Position3D, float3_zero);
    zox_set_ptr(terrain, PolyHeights, heights);
    dispose_PolyHeights(&heights);
    return terrain;
}

static entity zox_tst_capsule_spawn(ecs* world, entity terrain, float3 position) {
    const entity body = zox_new();
    zox_setv(body, Position3D, position);
    zox_setv(body, Velocity3D, float3_zero);
    zox_setv(body, Acceleration3D, float3_zero);
    zox_setv(body, Gravity3D, zox_default_gravity);
    zox_setv(body, Grounded, 0);
    zox_setv(body, Bounds3D, ((float3) { zox_tst_capsule_radius, zox_tst_capsule_height * 0.5f, zox_tst_capsule_radius }));
    zox_setv(body, Rotation3D, quaternion_identity);
    zox_add(body, Frictioned);
    zox_set_link(world, body, TerrainLink, terrain);

    spawn_capsule_collider(world, body, zox_tst_capsule_radius, zox_tst_capsule_height, float3_zero);
    return body;
}

static byte zox_tst_poly_landed(ecs* world) {
    const float3 position = zox_getv(zox_tst_capsule_body, Position3D);
    const float3 velocity = zox_getv(zox_tst_capsule_body, Velocity3D);
    return zox_getv(zox_tst_capsule_body, Grounded) && fabsf(position.y - 2.9f) < 0.02f && fabsf(velocity.y) < 0.01f;
}

static byte zox_tst_poly_land_ready(ecs* world) {
    if (!zox_tst_capsule_body) {
        if (!zox_tst_clock_prepare(&zox_tst_capsule_clock, zox_tst_physics_dt)) {
            return 0;
        }
        const float heights[] = { 2, 2, 2, 2 };
        const entity terrain = zox_tst_capsule_terrain(world, (int2) { 1, 1 }, (float2) { 8, 8 }, heights);
        zox_tst_capsule_body = zox_tst_capsule_spawn(world, terrain, (float3) { 4, 5, 4 });
        return 0;
    }
    zox_tst_clock_tick(&zox_tst_capsule_clock);
    return zox_tst_poly_landed(world) || zox_tst_physics_done(zox_tst_capsule_clock.time, 3);
}

static void zox_tst_poly_land_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_capsule_clock, "poly_land");
    zox_tst_check(zox_tst_poly_landed(world), "capsule should land at y=2.9 with zero vertical velocity");
    zox_tst_check(zox_log_errors_count == zox_tst_errors_start, "errors logged during poly_land");
}

static entity zox_tst_capsule_slope(ecs* world, float rise) {
    const float heights[] = { 0, 24 * rise, 0, 24 * rise };
    const entity terrain = zox_tst_capsule_terrain(world, (int2) { 1, 1 }, (float2) { 24, 12 }, heights);
    const float x = 12;
    const float centre = x * rise + 0.5f + zox_tst_capsule_radius * sqrtf(1 + rise * rise);
    return zox_tst_capsule_spawn(world, terrain, (float3) { x, centre, 6 });
}

static byte zox_tst_poly_slope_ready(ecs* world) {
    if (!zox_tst_capsule_body) {
        if (!zox_tst_clock_prepare(&zox_tst_capsule_clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_capsule_body = zox_tst_capsule_slope(world, 1);
        zox_tst_capsule_steep = zox_tst_capsule_slope(world, sqrtf(3));
        return 0;
    }
    zox_tst_clock_tick(&zox_tst_capsule_clock);
    zox_tst_check(!zox_getv(zox_tst_capsule_steep, Grounded), "60-degree slope should never ground the capsule");
    return zox_tst_physics_done(zox_tst_capsule_clock.time, 2);
}

static void zox_tst_poly_slope_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_capsule_clock, "poly_slope");
    const float3 walkable = zox_getv(zox_tst_capsule_body, Position3D);
    const float3 steep = zox_getv(zox_tst_capsule_steep, Position3D);
    zox_tst_check(zox_getv(zox_tst_capsule_body, Grounded) && fabsf(12 - walkable.x) * sqrtf(2) < 0.2f,
        "45-degree slope should ground the capsule with less than 0.2 m of drift (x=%f)", walkable.x);
    zox_tst_check(12 - steep.x > 1, "60-degree slope should slide more than 1 m (x=%f)", steep.x);
    zox_tst_check(zox_log_errors_count == zox_tst_errors_start, "errors logged during poly_slope");
}

static void zox_tst_poly_walk_drive(ecs* world, float speed) {
    const float3 velocity = zox_getv(zox_tst_capsule_body, Velocity3D);
    zox_setv(zox_tst_capsule_body, Velocity3D, ((float3) { speed, velocity.y, 0 }));
}

static byte zox_tst_poly_walk_ready(ecs* world) {
    const int2 cells = { 256, 128 };
    const float2 size = { 256, 128 };
    if (!zox_tst_capsule_body) {
        if (!zox_tst_clock_prepare(&zox_tst_capsule_clock, zox_tst_physics_dt)) {
            return 0;
        }
        zox_tst_capsule_island = zox_tst_capsule_terrain(world, cells, size, NULL);
        const PolyHeights* heights = zox_get(zox_tst_capsule_island, PolyHeights);
        const float y = poly_height_at(heights->value, cells, size, 20, 64);
        zox_tst_capsule_body = zox_tst_capsule_spawn(world, zox_tst_capsule_island, (float3) { 20, y + 0.9f, 64 });
        zox_tst_poly_walk_drive(world, 6);
        return 0;
    }
    zox_tst_clock_tick(&zox_tst_capsule_clock);
    const float3 position = zox_getv(zox_tst_capsule_body, Position3D);
    const PolyHeights* heights = zox_get(zox_tst_capsule_island, PolyHeights);
    const float surface = poly_height_at(heights->value, cells, size, position.x, position.z);
    zox_tst_check(position.y - 0.9f >= surface - 0.05f, "capsule under the island at frame %i", zox_tst_capsule_clock.frame);
    zox_tst_check(position.x >= 0 && position.x <= size.x && position.z >= 0 && position.z <= size.y,
        "capsule left the island grid at frame %i", zox_tst_capsule_clock.frame);
    zox_tst_poly_walk_drive(world, zox_tst_physics_done(zox_tst_capsule_clock.time, 5) ? 30 : 6);
    return zox_tst_physics_done(zox_tst_capsule_clock.time, 10);
}

static void zox_tst_poly_walk_check(ecs* world) {
    zox_tst_clock_check(&zox_tst_capsule_clock, "poly_walk");
    zox_tst_check(zox_getv(zox_tst_capsule_body, Position3D).x > 140, "capsule should travel across the island");
    zox_tst_check(zox_log_errors_count == zox_tst_errors_start, "errors logged during poly_walk");
}
