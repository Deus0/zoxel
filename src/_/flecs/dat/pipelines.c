entity zox_phase(ecs* world, const char* name) {
    entity e = ecs_entity(world, { .name = name });
    ecs_add_id(world, e, EcsPhase);
    return e;
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

// A barrier: every worker finishes the phases before it before any start the phases after (phases alone don't,
//  see doc/thread_safety.md), with one merge and nothing on the main thread:
//  - a writer system declares a staged write of ZoxSync ([out] ZoxSync(), no source), the waiter after it reads
//    ZoxSync on its entity ($this), flecs merges between them (a main thread system would make 2 merges)
//  - both multi threaded and empty, the systems either side stay multi threaded and in order
entity zox_sync_tag;

void zox_barrier_system(iter* it) {
    (void) it;
}

entity zox_phase_barrier(ecs* world, const char* name, entity after) {
    if (!zox_sync_tag) {
        zox_sync_tag = ecs_entity(world, { .name = "ZoxSync" });
        // the waiters' entity, so they're active (run) systems
        const entity holder = ecs_entity(world, { .name = "zox_sync" });
        ecs_add_id(world, holder, zox_sync_tag);
    }
    const entity phase = zox_phase_after_internal(world, name, after);
    char system_name[64];
    const char* suffixes[2] = { "_write", "_wait" };
    const char* queries[2] = { "[out] ZoxSync()", "[none] ZoxSync" };
    for (byte i = 0; i < 2; i++) {
        snprintf(system_name, sizeof(system_name), "%s%s", name, suffixes[i]);
        ecs_system_desc_t desc = { 0 };
        desc.entity = zox_system_entity(world, 0, system_name, phase);
        desc.query.expr = queries[i];
        desc.callback = zox_barrier_system;
        desc.multi_threaded = 1;
        const entity system = ecs_system_init(world, &desc);
        zox_system_on_new(world, system);
    }
    return phase;
}

// Debugs
entity zoxp_dbg_begin;
entity zoxp_begin;
// Who knows what this one does
entity zoxp_spawn;
entity zoxp_octree_write;
// a barrier: every worker finishes writing octrees before any reads them (see doc/thread_safety.md)
entity zoxp_octree_sync;
entity zoxp_octree_read;
// Apply Acc to Velocity
entity zoxp_pre_physics;
// Move Position/etc
entity zoxp_physics;
// trail, billboards
entity zoxp_trails;
// Layout -> Local Space
//entity zoxp_transform_sync;
entity zoxp_layouts;
// Hierarchy, Local -> World Space
//entity zoxp_transform_sync3;
entity zoxp_transform_hierarchy;
entity zoxp_billboards;
// World Space -> Transform Matrix
entity zoxp_transform_end;
// Transform + Projection -> ViewProjection
entity zoxp_cameras;
entity zoxp_gpu_upload;
entity zoxp_render;
entity zoxp_pre_end;
entity zoxp_end;
entity zoxp_post_end;
// End Debug
entity zoxp_dbg_end;

// Custom Phases / Pipeline
void initialize_zox_phases(ecs* world) {
    // NOTE: Keep in regular flecs phases for profiler
#ifdef flecs_profiler
    zox_phase_after(zoxp_dbg_begin, EcsOnStore);
#else
    zoxp_dbg_begin = zox_phase(world, "zoxp_dbg_begin");
#endif
    zox_phase_after(zoxp_begin, zoxp_dbg_begin);
    zox_phase_after(zoxp_spawn, zoxp_begin);
    zox_phase_after(zoxp_gpu_upload, zoxp_spawn);
    zox_phase_after(zoxp_octree_write, zoxp_gpu_upload);
    // octree writes finish before reads
    zoxp_octree_sync = zox_phase_barrier(world, "zoxp_octree_sync", zoxp_octree_write);
    zox_phase_after(zoxp_octree_read, zoxp_octree_sync);
    zox_phase_after(zoxp_pre_physics, zoxp_octree_read);
    zox_phase_after(zoxp_physics, zoxp_pre_physics);
    // the transform chain, each step multi threaded, a barrier before each: physics -> trails -> layouts ->
    //  hierarchy -> billboards -> matrices, each reads what the step before wrote
    const entity trails_sync = zox_phase_barrier(world, "zoxp_trails_sync", zoxp_physics);
    zox_phase_after(zoxp_trails, trails_sync);
    const entity layouts_sync = zox_phase_barrier(world, "zoxp_layouts_sync", zoxp_trails);
    zox_phase_after(zoxp_layouts, layouts_sync);
    const entity hierarchy_sync = zox_phase_barrier(world, "zoxp_hierarchy_sync", zoxp_layouts);
    zox_phase_after(zoxp_transform_hierarchy, hierarchy_sync);
    const entity billboards_sync = zox_phase_barrier(world, "zoxp_billboards_sync", zoxp_transform_hierarchy);
    zox_phase_after(zoxp_billboards, billboards_sync);
    const entity transform_end_sync = zox_phase_barrier(world, "zoxp_transform_end_sync", zoxp_billboards);
    zox_phase_after(zoxp_transform_end, transform_end_sync);
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
#define zoxp_update zoxp_pre_physics
#define zoxp_initialize zoxp_state
#define zoxp_reset zoxp_pre_end
// Creation
#define zoxp_destroy zoxp_pre_end
// Iterate Time
#define zoxp_time zoxp_end
// Push rendering to SDL Window
#define zoxp_render_end zoxp_end
// NOTE: Remove (Components) before spawn so it lasts a frame
#define zoxp_remove zoxp_reset // zoxp_begin
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
// #define zoxp_voxels_mesh zoxp_octree_read
// Write to mesh from voxels
#define zoxp_voxels_lights zoxp_octree_read
// Queues
#define zoxp_queue_process zoxp_physics
// #define zoxp_queue_clear zoxp_octree_write

// Old: EcsOnUpdate

/* EcsOnLoad
 * EcsPostLoad
 * EcsPreUpdate
 * EcsOnUpdate
 * EcsOnValidate
 * EcsPostUpdate
 * EcsPreStore
 * EcsOnStore
 */
