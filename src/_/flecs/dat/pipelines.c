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
entity zoxp_end;

// Custom Phases / Pipeline
void initialize_zox_phases(ecs* world) {
    zox_phase_after(zoxp_pre_spawn, EcsOnStore);
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
    zox_phase_after(zoxp_end, zoxp_render);
}

/* EcsOnLoad
 * EcsPostLoad
 * EcsPreUpdate
 * EcsOnUpdate
 * EcsOnValidate
 * EcsPostUpdate
 * EcsPreStore
 * EcsOnStore
 */
// 1) Spawn
// 2) Main - Multithreaded
// 3) GPU Uploads + Rendering
#define zoxp_dbg_begin EcsOnLoad
#define zoxp_dbg_end zoxp_end
// Iterate Time
#define zoxp_time zoxp_end
// Push rendering to SDL Window
#define zoxp_render_end zoxp_end
// NOTE: Remove (Components) before spawn so it lasts a frame
#define zoxp_remove zoxp_pre_spawn
// regular sync points
// #define zoxp_spawn EcsPreUpdate
#define zoxp_initialize EcsOnUpdate
#define zoxp_update EcsOnUpdate
#define zoxp_post_update EcsOnValidate
#define zoxp_load EcsOnValidate
#define zoxp_save EcsOnValidate
// system based
#define zoxp_state EcsOnUpdate
#define zoxp_reset EcsPostUpdate
#define zoxp_destroy EcsOnUpdate
// doesnt seem to mind if its in same frame as zoxp_cameras
// Inputs
#define zoxp_inputs_extract zoxp_spawn
#define zoxp_inputs_reset zoxp_reset
#define zoxp_inputs_enable zoxp_update
#define zoxp_inputs_update zoxp_update
// (VoxelNode) Queue
// TODO: Process Queue for Octree in frame a fter, perhaps a second stack?
#define zoxp_queue_add zoxp_update
#define zoxp_queue_process zoxp_update
#define zoxp_queue_pre_clear EcsOnValidate
#define zoxp_queue_pre_post_clear EcsOnValidate
#define zoxp_queue_clear zoxp_octree_write // EcsOnValidate
// Voxels
#define zoxp_voxels_generate zoxp_octree_write    // Write to voxels
#define zoxp_voxels_mesh zoxp_octree_read      // Write to mesh from voxels
#define zoxp_voxels_lights zoxp_octree_read      // Write to mesh from voxels

// Pipeline: Movement to Render Pipeline
//  movement → transform → matrix → camera → render
// NOTE: Change Velocity
// #define zoxp_physics_apply zoxp_pre_physics
// NOTE: Moves Transform
// #define zoxp_physics zoxp_physics
// NOTE: Transforms handle Hierarchy and Matrix
// #define zoxp_pre_transforms zoxp_pre_transform
// #define zoxp_transforms zoxp_transform
// #define zoxp_transforms_matrix zoxp_transform_end
// NOTE: Calculates ViewMatrix
// #define zoxp_cameras zoxp_camera_matrix
// #define zoxp_gpu_upload zoxp_render_upload // EcsPreStore
// #define zoxp_rendering zoxp_render
// #define zoxp_rendering_end zoxp_render_end

