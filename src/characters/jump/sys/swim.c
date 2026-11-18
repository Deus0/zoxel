// swimming, while InWater (terrain/collisions/eve/water_splash.c, its lower body in water)
//  - water holds it up against most of its gravity (it sinks slowly) and drags its velocity (it slows, falls in softly)
//  - a jump in water is a stroke up (jump_state_swim, or the trigger itself: the player's jump runs after jump_system)
//  - walking is slower (player3d_move_system: swim_speed)
//  - at the surface beside a shore, jump jumps out (swim_at_shore)
#define swim_buoyancy 0.85f     // of its gravity held up
#define swim_drag 3.0f          // velocity lost a second, per meter a second
#define swim_power 12.0f        // a stroke up, meters a second a second: about 3.5 up at most
#define swim_speed 0.5f         // of the walk's top speed

// at the water's surface (air over its block), a solid block beside the air: a shore to climb onto
static inline byte swim_at_shore(ecs* world, entity e) {
    const entity terrain = zox_get_link(world, e, TerrainLink);
    if (!zox_valid(terrain) || !zox_has(e, InsideBlockPosition)) {
        return 0;
    }
    const int3 above = int3_add(zox_getv(e, InsideBlockPosition), int3_up);
    if (terrain_voxel_at(world, terrain, above) != 0) {
        return 0;
    }
    WaterLevels levels;
    water_levels_get(world, zox_get_parent(world, terrain), &levels);
    const int3 sides[4] = { int3_left, int3_right, int3_forward, int3_backward };
    for (byte i = 0; i < 4; i++) {
        const int voxel = terrain_voxel_at(world, terrain, int3_add(above, sides[i]));
        if (voxel > 0 && !voxel_is_water(&levels, voxel)) {
            return 1;
        }
    }
    return 0;
}

void swim_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(JumpState);
    zox_sys_in(Velocity3D);
    zox_sys_in(Gravity3D);
    zox_sys_out(Acceleration3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(JumpState, jump);
        zox_sys_i(Velocity3D, velocity);
        zox_sys_i(Gravity3D, gravity);
        zox_sys_o(Acceleration3D, acceleration);
        float3 force = float3_scale(gravity->value, -swim_buoyancy);
        float3_add_float3_p(&force, float3_scale(velocity->value, -swim_drag));
        if (jump->value == jump_state_trigger || jump->value == jump_state_swim) {
            force.y += swim_power;
        }
        float3_add_float3_p(&acceleration->value, force);
    }
    zox_sys_on_end();
} zoxd_system(swim_system);
