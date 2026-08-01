void emit_particle3Ds_slow(ecs *world, float3 emit_position, float3 bounds, int spawn_count, color colorr) {
    // const float3 spawn_bounds = { 0.4f, 0.8, 0.4f };
    float3 acb = { 0.4f, 0.8, 0.4f };
    for (int i = 0; i < spawn_count; i++) {
        float3 position = emit_position;
        float3 acceleration = float3_zero;
        float3_add_float3_p(&position, (float3) {
            ((rand() % 101) / 100.0f) * bounds.x - (bounds.x / 2.0f),
            ((rand() % 101) / 100.0f) * bounds.y - (bounds.y / 2.0f),
            ((rand() % 101) / 100.0f) * bounds.z - (bounds.z / 2.0f)
        });
        float3_add_float3_p(&acceleration, (float3) {
            frand_range(-acb.x, acb.x),
            frand_range(-acb.y, acb.y),
            frand_range(-acb.z, acb.z)
        });
        float4 particle_color = color_to_float4(colorr);
        // (float4) { (rand() % 100) * 0.01f, (rand() % 100) * 0.01f, (rand() % 100) * 0.01f, 0 };
        // float4_multiply_float_p(&particle_color, 0.3f);
        particle_color.w += -0.2f + (rand() % 100) * 0.01f * 0.2f;
        spawn_particle3(world, position, acceleration, float4_to_color(particle_color));
    }
}


// #define zox_bulk_spawning
void emit_particle3Ds(ecs *world, const float3 spawn_position, const int spawn_count) {
    float3 spawn_bounds = { 0.4f, 0.8, 0.4f };
    const float2 velocityBounds = { 0.03f, 0.2f };
    const float2 scaleBounds = { 0.02f, 0.13f };
    const double2 lifeTime = { 0.1f, 2.0f };
    const float2 brightnessBounds = { 0.2f, 0.8f };
    Position3D *positions = malloc(sizeof(Position3D) * spawn_count);
    Velocity3D *velocity3Ds = malloc(sizeof(Velocity3D) * spawn_count);
    Scale1 *scale1Ds = malloc(sizeof(Scale1) * spawn_count);
    Brightness *brightnesses = malloc(sizeof(Brightness) * spawn_count);
    DestroyInTime *destroyInTimes = malloc(sizeof(DestroyInTime) * spawn_count);
    for (int i = 0; i < spawn_count; i++) {
        positions[i].value = spawn_position;
        float3_add_float3_p(&positions[i].value, (float3) {
            ((rand() % 101) / 100.0f) * spawn_bounds.x - (spawn_bounds.x / 2.0f),
            ((rand() % 101) / 100.0f) * spawn_bounds.y - (spawn_bounds.y / 2.0f),
            ((rand() % 101) / 100.0f) * spawn_bounds.z - (spawn_bounds.z / 2.0f)
        });
        velocity3Ds[i].value = (float3) {
            ((rand() % 101) / 100.0f) * 1.0f - 0.5f,
            ((rand() % 101) / 100.0f) * 1.0f - 0.5f,
            ((rand() % 101) / 100.0f) * 1.0f - 0.5f
        };
        velocity3Ds[i].value = float3_normalize(velocity3Ds[i].value);
        velocity3Ds[i].value = float3_scale(velocity3Ds[i].value, (velocityBounds.y - velocityBounds.x));
        if (velocity3Ds[i].value.x < 0) velocity3Ds[i].value.x -= velocityBounds.x;
        else velocity3Ds[i].value.x += velocityBounds.x;
        if (velocity3Ds[i].value.y < 0) velocity3Ds[i].value.y -= velocityBounds.x;
        else velocity3Ds[i].value.y += velocityBounds.x;
        if (velocity3Ds[i].value.z < 0) velocity3Ds[i].value.z -= velocityBounds.x;
        else velocity3Ds[i].value.z += velocityBounds.x;
        scale1Ds[i].value = scaleBounds.x + ((rand() % 101) / 100.0f) * (scaleBounds.y - scaleBounds.x);
        brightnesses[i].value = brightnessBounds.x + ((rand() % 101) / 100.0f) * (brightnessBounds.y - brightnessBounds.x);
        destroyInTimes[i].value = lifeTime.x + ((rand() % 101) / 100.0f) *  (lifeTime.y - lifeTime.x);
    }
    // const entity *particlesArray =
    ecs_bulk_init(world, &(ecs_bulk_desc_t) {
        .count = spawn_count,
        .ids = {
            ecs_pair(EcsIsA, prefab_particle3D),
            ecs_id(Position3D),
            ecs_id(Rotation3D),
            ecs_id(Scale1),
            ecs_id(Velocity3D),
            ecs_id(Acceleration3D),
            ecs_id(Brightness),
            ecs_id(DestroyInTime)
        },
        // provide data for each id
        .data = (void*[]) {
            NULL,           // Prefab pair, what is it used for?
            positions,
            NULL,           // Rotation3D
            scale1Ds,
            velocity3Ds,
            NULL,           // Acceleration3D
            brightnesses,
            destroyInTimes
        }
    });
    free(positions);
    free(scale1Ds);
    free(velocity3Ds);
    free(brightnesses);
    free(destroyInTimes);
}


zox_sys2(Particle3DEmitSystem) {
    byte is_bulk_spawn = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(ParticleEmitRate);
    zox_sys_in(Bounds3D)
    zox_sys_in(Color)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(ParticleEmitRate, rate);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_i(Color, colorr);
        if (is_bulk_spawn) {
            emit_particle3Ds(world, position->value, rate->value);
        } else {
            emit_particle3Ds_slow(world, position->value, bounds->value, rate->value, colorr->value);
        }
    }
} zox_sys_end(Particle3DEmitSystem);