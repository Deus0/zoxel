// #include "printer.c"
#include "lines.c"
// #include "sides.c"
#include "refresh.c"

void set_light_debug_systems(ecs* world, byte enabled) {
    zox_debug_lights = enabled;
    zox_set_enabled(LightNodeDebugSystem, enabled);
}

void zox_dbg_cycle_light_debug(ecs *world, ClickEventData data) {
    // zox_debug_lights = !zox_debug_lights;
    /*zox_debug_lights++;
    if (zox_debug_lights > terrain_depth + 1) {
        zox_debug_lights = 0;
    }*/
    if (!zox_debug_lights) {
        zox_debug_lights = terrain_depth + 1;
    } else {
        zox_debug_lights = 0;
    }
    set_light_debug_systems(world, zox_debug_lights);
    if (!zox_debug_lights) {
        zox_log("Debug Lights Disabled");
    } else {
        zox_log("+ Debug Lights Depth [%i]", (zox_debug_lights - 1));
    }
}

void zoxd_systems_light3_debug(ecs* world) {
    zox_system_1(
        LightNodeDebugSystem,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxelNode,
        [in] lights3.LightNode,
        [in] rendering.RenderDepth,
        [none] chunks.Chunk,
    );
    set_light_debug_systems(world, zox_debug_lights);
}