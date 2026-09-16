entity zox_phase(ecs* world, const char* name) {
    return ecs_entity(world, {
        .name = name,
        .add = ecs_ids(EcsPhase)
    });
}

entity zox_phase_after_internal(
    ecs* world,
    const char* name,
    entity after)
{
    entity phase = zox_phase(world, name);
    ecs_add_pair(world, phase, EcsDependsOn, after);
    return phase;
}

#define zox_phase_after(T, phase)\
    T = zox_phase_after_internal(world, #T, phase);

// Debugs
entity zoxp_dbg_begin;
entity zoxp_dbg_end;
entity zoxp_pre_spawn;
// Who knows what this one does
entity zoxp_spawn;
entity zoxp_octree_write;
entity zoxp_octree_read;
// Apply Acc to Velocity
entity zoxp_pre_physics;
// Move Position/etc
entity zoxp_physics;
// trail, billboards
entity zoxp_trails;
// Layout -> Local Space
entity zoxp_layouts;
// Hierarchy, Local -> World Space
entity zoxp_transform_hierarchy;
// World Space -> Transform Matrix
entity zoxp_transform_end;
// Transform + Projection -> ViewProjection
entity zoxp_cameras;
entity zoxp_gpu_upload;
entity zoxp_render;
entity zoxp_pre_end;
entity zoxp_end;
entity zoxp_post_end;

// Custom Phases / Pipeline
void initialize_zox_phases(ecs* world) {
    zoxp_dbg_begin = zox_phase(world, "zoxp_dbg_begin");
    zox_phase_after(zoxp_pre_spawn, zoxp_dbg_begin);
    zox_phase_after(zoxp_spawn, zoxp_pre_spawn);
    zox_phase_after(zoxp_gpu_upload, zoxp_spawn);
    zox_phase_after(zoxp_octree_write, zoxp_gpu_upload);
    zox_phase_after(zoxp_octree_read, zoxp_octree_write);
    zox_phase_after(zoxp_pre_physics, zoxp_octree_read);
    zox_phase_after(zoxp_physics, zoxp_pre_physics);
    zox_phase_after(zoxp_trails, zoxp_physics);
    zox_phase_after(zoxp_layouts, zoxp_trails);
    zox_phase_after(zoxp_transform_hierarchy, zoxp_layouts);
    zox_phase_after(zoxp_transform_end, zoxp_transform_hierarchy);
    zox_phase_after(zoxp_cameras, zoxp_transform_end);
    zox_phase_after(zoxp_render, zoxp_cameras);
    zox_phase_after(zoxp_pre_end, zoxp_render);
    zox_phase_after(zoxp_end, zoxp_pre_end);
    zox_phase_after(zoxp_post_end, zoxp_end);
    zox_phase_after(zoxp_dbg_end, zoxp_post_end);
}

// States
#define zoxp_state zoxp_pre_physics
#define zoxp_post_update zoxp_physics
#define zoxp_update zoxp_state
#define zoxp_initialize zoxp_state
#define zoxp_reset zoxp_pre_end
// Creation
#define zoxp_destroy zoxp_pre_end
// Iterate Time
#define zoxp_time zoxp_end
// Push rendering to SDL Window
#define zoxp_render_end zoxp_end
// NOTE: Remove (Components) before spawn so it lasts a frame
#define zoxp_remove zoxp_reset // zoxp_pre_spawn
// Inputs
#define zoxp_inputs_extract zoxp_spawn
#define zoxp_inputs_reset zoxp_reset
#define zoxp_inputs_enable zoxp_update
#define zoxp_inputs_update zoxp_update
// IO
#define zoxp_load zoxp_post_update
#define zoxp_save zoxp_post_update
// Voxels
// Write to voxels
#define zoxp_voxels_generate zoxp_octree_write
// Write to mesh from voxels
#define zoxp_voxels_mesh zoxp_octree_read
// Write to mesh from voxels
#define zoxp_voxels_lights zoxp_octree_read
// QUeues
// #define zoxp_queue_add zoxp_update
#define zoxp_queue_process zoxp_physics
// #define zoxp_queue_pre_clear zoxp_post_update
// #define zoxp_queue_pre_post_clear zoxp_post_update
#define zoxp_queue_clear zoxp_octree_write

// Old: EcsOnUpdate

// 1) Spawn
// 2) Main - Multithreaded
// 3) GPU Uploads + Rendering
// regular sync points
// #define zoxp_spawn EcsPreUpdate
// doesnt seem to mind if its in same frame as zoxp_cameras
// (VoxelNode) Queue
// TODO: Process Queue for Octree in frame a fter, perhaps a second stack?
/* EcsOnLoad
 * EcsPostLoad
 * EcsPreUpdate
 * EcsOnUpdate
 * EcsOnValidate
 * EcsPostUpdate
 * EcsPreStore
 * EcsOnStore
 */