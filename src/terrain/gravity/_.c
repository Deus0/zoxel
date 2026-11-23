/*
 * +--------------------------------------------------------------+
 * | Zox Module: Gravities                                        |
 * |                                                              |
 * |  Gravity Fields - Gravity Cubes - Bodies Pulled By Them      |
 * |                                                              |
 * |  Uses: Chunks - Block Datas - Octrees - Physics3             |
 * |                                                              |
 * +--------------------------------------------------------------+
 *
 *  - a terrain chunk's GravityOctree: where gravity pulls, a byte a voxel (0 the realm's own, down; else a
 *    direction, fun.c), beside its voxels as its LightOctree is
 *  - a gravity cube (BlockGravity) switched on (its chunk's ActiveOctree, as an open door: a right click, block_toggle)
 *    flips gravity in a sphere round it, gravity_radius blocks; where spheres overlap each flips it again
 *  - the field is worked out from the cubes switched on near a chunk (gravity_rebuild): when a chunk's voxels change
 *    (VoxelNodeDirty: generated, loaded, edited) and when a cube is toggled (gravity_dirty_around), by an observer
 *    (GravityDirty), on the main thread
 *  - a body with TerrainGravity (characters) takes its Gravity3D from the field where it is, every frame
 *    (terrain_gravity_system), and stands on it: its up turned to its gravity's (gravity_upright_system)
 *  - debug: the gizmos' Gravity button draws the fields round the camera (dbg.c)
 */
#include "com.c"
#include "fun.c"
#include "sys.c"
#include "dbg.c"

void prefab_add_gravity(ecs* world) {
    zox_set(prefab_chunk_terrain, GravityOctree, { .value = gravity_default });
}

void import_gravities(ecs* world) {
    zox_module(gravities);
    zox_components_gravities(world);
    zox_on_add(on_gravity_voxels, [none] chunks3.VoxelNodeDirty, [none] gravities.GravityOctree);
    zox_on_add(on_gravity_dirty, [none] gravities.GravityDirty);
    zox_system(
        terrain_gravity_system,
        zoxp_update,
        [in] transforms3.Position3D,
        [out] physics3.Gravity3D,
        [none] gravities.TerrainGravity
    );
    zox_system(
        gravity_upright_system,
        zoxp_update,
        [in] physics3.Gravity3D,
        [out] transforms3.Rotation3D,
        [none] gravities.TerrainGravity,
        [none] !physics.DisableMovement     // laid on a bed (beds), held by something else
    );
    zoxd_systems_gravity_debug(world);
    add_hook_spawn_prefabs(prefab_add_gravity);
}
