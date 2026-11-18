// a 3D camera inside a water block is Underwater (XR's eyes: xr/fun/eye_render.c checks its own): camera_render3_system draws its view in a thick blue fog (the sky
//  too, skybox.frag), and the first main camera's mutes sounds (sound_underwater, sounds/set/volume.c)
//  - the block at the camera itself, not the body's (the eyes can be over the surface while swimming)
// is a world position inside water (0 if its chunk isn't loaded): a flowing block's only up to its level, unless
//  water is over it
static inline byte terrain_water_at(ecs* world, entity terrain, float3 position) {
    if (!zox_valid(terrain) || !zox_has(terrain, BlockScale)) {
        return 0;
    }
    WaterLevels levels;
    if (!water_levels_get(world, zox_get_parent(world, terrain), &levels)) {
        return 0;
    }
    const float scale = zox_getv(terrain, BlockScale);
    const int3 block = real_position_to_block_position(position, scale);
    const int voxel = terrain_voxel_at(world, terrain, block);
    if (!voxel_is_water(&levels, voxel)) {
        return 0;
    }
    const byte level = levels.level[voxel];
    if (level == zox_water_level_source || voxel_is_water(&levels, terrain_voxel_at(world, terrain, int3_add(block, int3_up)))) {
        return 1;
    }
    const float height = position.y / scale - block.y;
    return height < level / (float) zox_water_level_source;
}

void camera_underwater_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const entity terrain = local_terrain;
    if (!zox_valid(terrain) || !zox_has(terrain, BlockScale)) {
        sound_underwater = 0;   // back at the menu
        return;
    }
    zox_sys_begin();
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        const byte under = terrain_water_at(world, terrain, position->value);
        if (e == main_cameras[0]) {
            sound_underwater = under;
        }
        if (under == zox_has(e, Underwater)) {
            continue;
        }
        if (under) {
            zox_add(e, Underwater);
        } else {
            zox_remove(e, Underwater);
        }
    }
    zox_sys_on_end();
} zoxd_system(camera_underwater_system);
