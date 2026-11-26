typedef struct {
    const float* heights;
    int2 cells;
    float2 size;
} poly_collision_grid;

static void poly_clip_into(float3* vector, float3 normal) {
    const float into = float3_dot(*vector, normal);
    if (into < 0) {
        *vector = float3_sub(*vector, float3_scale(normal, into));
    }
}

static byte poly_resolve_capsule(poly_collision_grid grid, float radius, float half_axis, float3* centre,
    float3* velocity, float3* step, float3* support) {
    const float walkable_y = 0.70710677f;
    const float3 axis = { 0, half_axis, 0 };
    byte grounded = 0;
    for (int i = 0; i < 3; i++) {
        float3 push, normal;
        if (!poly_capsule_push(grid.heights, grid.cells, grid.size, float3_sub(*centre, axis),
            float3_add(*centre, axis), radius, &push, &normal)) {
            break;
        }
        *centre = float3_add(*centre, push);
        poly_clip_into(velocity, normal);
        poly_clip_into(step, normal);
        if (normal.y >= walkable_y) {
            grounded = 1;
            *support = normal;
        }
    }
    const float surface = poly_height_at(grid.heights, grid.cells, grid.size, centre->x, centre->z);
    const float bottom = centre->y - half_axis - radius;
    if (bottom < surface) {
        centre->y += surface - bottom;
        const float3 normal = poly_normal_at(grid.heights, grid.cells, grid.size, centre->x, centre->z);
        poly_clip_into(velocity, normal);
        poly_clip_into(step, normal);
        if (normal.y >= walkable_y) {
            grounded = 1;
            *support = normal;
        }
    }
    return grounded;
}

static byte poly_move_capsule(poly_collision_grid grid, float radius, float half_axis, float dt,
    float3* centre, float3* velocity, float3* support) {
    const float3 move = float3_scale(*velocity, dt);
    const float distance = float3_length(move);
    float3 step = float3_zero;
    byte grounded = 0;
    if (distance > radius) {
        const int steps = (int) ceilf(distance / radius);
        step = float3_scale(move, 1.0f / steps);
        *centre = float3_sub(*centre, move);
        for (int i = 0; i < steps; i++) {
            *centre = float3_add(*centre, step);
            grounded = poly_resolve_capsule(grid, radius, half_axis, centre, velocity, &step, support);
        }
    } else {
        grounded = poly_resolve_capsule(grid, radius, half_axis, centre, velocity, &step, support);
    }
    return grounded;
}

void poly_capsule_collision_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(Position3D);
    zox_sys_out(Velocity3D);
    zox_sys_out(Grounded);
    const float dt = zox_delta_time > zox_physics_max_delta_time ? 0 : (float) zox_delta_time;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // Non-fragmenting links are read on the entity.
        const entity collider = zox_get_link(world, e, ColliderLink);
        if (!zox_valid(collider) || !zox_has(collider, Capsule)) {
            continue;
        }
        const entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain) || !zox_has(terrain, PolyTerrain)) {
            continue;
        }
        const PolyHeights* heights = zox_get(terrain, PolyHeights);
        const PolyCells* cells = zox_get(terrain, PolyCells);
        const Size2* size = zox_get(terrain, Size2);
        const Position3D* origin = zox_get(terrain, Position3D);
        const RadiusFloat* radius = zox_get(collider, RadiusFloat);
        const HeightFloat* height = zox_get(collider, HeightFloat);
        const LocalPosition3D* offset = zox_get(collider, LocalPosition3D);
        if (!heights || !cells || !size || !origin || !radius || !height || !offset ||
            !(radius->value > 0 && height->value >= 2 * radius->value) ||
            cells->value.x <= 0 || cells->value.y <= 0 || size->value.x <= 0 || size->value.y <= 0 ||
            !heights->value || (size_t) heights->length != ((size_t) cells->value.x + 1) * ((size_t) cells->value.y + 1)) {
            continue;
        }
        zox_sys_o(Position3D, position);
        zox_sys_o(Velocity3D, velocity);
        zox_sys_o(Grounded, grounded);
        float3 centre = float3_add(float3_sub(position->value, origin->value), offset->value);
        const poly_collision_grid grid = { heights->value, cells->value, size->value };
        const float body_dt = dt > 0 && zox_has(e, ClockDelta) ? zox_getv(e, ClockDelta) : dt;
        const byte was_grounded = grounded->value;
        float3 support;
        grounded->value = poly_move_capsule(grid, radius->value, height->value * 0.5f - radius->value, body_dt,
            &centre, &velocity->value, &support);
        // Static friction on walkable ground cancels the tangential velocity added by gravity this frame.
        if (grounded->value && zox_has(e, Frictioned) && zox_has(e, Acceleration3D) &&
            zox_has(e, Gravity3D) && !zox_has(e, DisableGravity)) {
            const physics_step drag = physics_step_new(was_grounded ? ground_friction3 : air_friction3, body_dt);
            const float3 gravity = zox_getv(e, Gravity3D);
            const float3 tangent = float3_sub(gravity, float3_scale(support, float3_dot(gravity, support)));
            velocity->value = float3_sub(velocity->value, float3_scale(tangent, drag.g));
        }
        position->value = float3_add(float3_sub(centre, offset->value), origin->value);
    }
    zox_sys_on_end();
} zoxd_system(poly_capsule_collision_system);
