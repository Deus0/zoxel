// Footsteps: a character walking on the ground plays the block underfoot's hit sound each stride
//  (doc/sounds.md, the block's HitSoundLink, softer), characters tagged Footsteps (players, for now)
//  - FootstepDistance adds up the ground walked, a stride is about its body's height
#define footstep_stride 1.6f    // of its half height (Bounds3D.y)
#define footstep_min_speed 0.2f // walking, not drifting

void footsteps_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Grounded);
    zox_sys_in(Velocity3D);
    zox_sys_in(Position3D);
    zox_sys_in(Bounds3D);
    zox_sys_out(FootstepDistance);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Grounded, grounded);
        zox_sys_i(Velocity3D, velocity);
        zox_sys_i(Position3D, position);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_o(FootstepDistance, distance);
        const float speed = sqrtf(velocity->value.x * velocity->value.x + velocity->value.z * velocity->value.z);
        if (!grounded->value || speed < footstep_min_speed) {
            continue;
        }
        distance->value += speed * (float) zox_delta_time;
        const float stride = footstep_stride * bounds->value.y;
        if (distance->value < stride) {
            continue;
        }
        distance->value -= stride;
        // the block under its feet, in its realm's blocks
        const entity terrain = zox_get_link(world, e, TerrainLink);
        const entity realm = zox_get_link(world, e, RealmLink);
        if (!zox_valid(terrain) || !zox_valid(realm) || !zox_has(realm, BlockLinks)) {
            continue;
        }
        const float scale = zox_getv(terrain, BlockScale);
        const float3 feet = { position->value.x, position->value.y - bounds->value.y - scale * 0.5f, position->value.z };
        const int voxel = terrain_voxel_at(world, terrain, real_position_to_block_position(feet, scale));
        const BlockLinks* blocks = zox_get(realm, BlockLinks);
        if (voxel <= 0 || voxel > blocks->length) {
            continue;
        }
        spawn_footstep_sound(world, blocks->value[voxel - 1], feet);
    }
    zox_sys_on_end();
} zoxd_system(footsteps_system);
