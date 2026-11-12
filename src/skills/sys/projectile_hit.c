// Projectiles raycast along their velocity each frame, damaging the first character or terrain block hit
//  - the ray covers the distance moved this frame, so fast projectiles can't pass through things
//  - the user who fired it is never hit
void projectile_hit_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    const float min_speed = 0.01f;
    const float ray_margin = 0.05f;     // a little ahead, as the projectile has size
    const float max_delta = 0.1f;
    float delta = zox_delta_time;
    if (delta > max_delta) {
        delta = max_delta;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Velocity3D);
    zox_sys_in(Damage);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Velocity3D, velocity);
        zox_sys_i(Damage, damage);
        const float speed = float3_length(velocity->value);
        if (speed < min_speed) {
            continue;
        }
        entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain)) {
            continue;
        }
        entity realm = zox_get_parent(world, terrain);
        if (!zox_valid(realm)) {
            continue;
        }
        const entity user = zox_get_link(world, e, ProjectileUser);
        const entity skill = zox_get_link(world, e, ProjectileSkill);
        const float3 ray_normal = float3_scale(velocity->value, 1.0f / speed);
        const float ray_length = speed * delta + ray_margin;
        zox_geter(realm, BlockLinks, voxels);
        zox_geter(terrain, ChunkLinks, chunks);
        const float block_scale = zox_getv(terrain, BlockScale);
        const byte terrain_depth = zox_getv(terrain, NodeDepth);
        const int3 size = int3_single(octree_size(terrain_depth));
        RaycastVoxelData data = { 0 };
        CharacterRaycast character_raycast = { 0 };
        const byte result = raycast_voxel_node(
            world,
            user,
            voxels,
            chunks,
            int3_zero,
            float3_zero,
            terrain_depth,
            size,
            0,
            position->value,
            ray_normal,
            int3_zero,
            block_scale,
            ray_length,
            &data,
            &character_raycast);
        if (result == rayhit_none) {
            continue;
        }
        const entity hit = data.chunk;
        if (result == rayhit_character) {
            if (zox_valid(hit) &&
                zox_has(hit, Character3) &&
                !zox_has(hit, Dead))
            {
                damage_character(
                    world,
                    user,
                    hit,
                    skill,
                    damage->value,
                    ray_normal);
            }
        } else if (data.voxel &&
            zox_valid(data.hit_block) &&
            zox_valid(hit) &&
            zox_has(hit, TerrainChunk))
        {
            damage_terrain_block(world, hit, data.positionl, damage->value);
        }
        if (dbg_log) {
            zox_log("Projectile [%s] hit [%s] result [%i]",
                zox_getn(e),
                zox_getn(hit),
                result);
        }
        zox_delete(e);
    }
    zox_sys_on_end();
} zoxd_system(projectile_hit_system);
